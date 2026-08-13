// Smart Cosplay Prop — heartbeat pulse + slash animation
//
// Behavior:
//   - No finger on MAX30102        -> LEDs off
//   - Finger present, no BPM yet   -> slow "searching" pulse
//   - Valid BPM computed           -> pulse speed scales with heart rate range
//   - IMU detects a slash motion   -> interrupts pulsing, sweeps LEDs on from
//                                      bottom to tip, then a trailing "off" sweep
//                                      follows ~1s behind, then resumes pulsing
//
// Libraries needed (Library Manager):
//   - Adafruit NeoPixel
//   - SparkFun MAX3010x Pulse and Proximity Sensor Library (provides MAX30105.h + heartRate.h)
// MPU6500 uses raw Wire register access, no library needed.

#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include "MAX30105.h"
#include "heartRate.h"

// ---------- Pin / hardware config ----------
#define SDA_PIN     21
#define SCL_PIN     22
#define LED_PIN     18
#define NUM_LEDS    12      // set to your actual LED count, ordered hilt -> tip

const int MPU_ADDR = 0x68;
MAX30105 particleSensor;
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Colors — tweak to taste
uint32_t COLOR_PULSE = strip.Color(0, 80, 255);   // blue pulse
uint32_t COLOR_SLASH = strip.Color(80, 160, 255); // brighter blue for slash sweep

// ---------- Heart rate detection ----------
const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
int beatAvg = 0;
bool fingerPresent = false;

// ---------- State machine ----------
enum State { IDLE_OFF, SEARCHING, BPM_PULSE, SLASH_ANIM };
State currentState = IDLE_OFF;
State stateBeforeSlash = IDLE_OFF;

// ---------- Slash detection ----------
float GYRO_SLASH_THRESHOLD = 300.0; // deg/s, tune based on real swing tests
unsigned long lastSlashTime = 0;
const unsigned long SLASH_COOLDOWN = 800; // ms, prevents re-trigger mid-animation

// ---------- Slash animation timing ----------
unsigned long slashStartTime = 0;
const unsigned long SLASH_STEP_MS = 40;   // time between each LED turning on
const unsigned long SLASH_TRAIL_MS = 1000; // delay before an LED turns back off

// ================= MPU6500 helpers =================

bool mpuPresent() {
  Wire.beginTransmission(MPU_ADDR);
  return (Wire.endTransmission() == 0);
}

void mpuWake() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();
}

void mpuReadGyro(float &gxd, float &gyd, float &gzd) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x43); // GYRO_XOUT_H
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);

  int16_t gx = Wire.read() << 8 | Wire.read();
  int16_t gy = Wire.read() << 8 | Wire.read();
  int16_t gz = Wire.read() << 8 | Wire.read();

  gxd = gx / 131.0;
  gyd = gy / 131.0;
  gzd = gz / 131.0;
}

bool checkForSlash() {
  float gx, gy, gz;
  mpuReadGyro(gx, gy, gz);
  float magnitude = sqrt(gx * gx + gy * gy + gz * gz);

  if (magnitude > GYRO_SLASH_THRESHOLD && millis() - lastSlashTime > SLASH_COOLDOWN) {
    lastSlashTime = millis();
    return true;
  }
  return false;
}

// ================= Pulse animation =================

unsigned long pulsePeriod(int bpm) {
  // Discrete BPM ranges -> pulse speed. Tune these to taste.
  if (bpm < 60)  return 1800;
  if (bpm < 90)  return 1200;
  if (bpm < 120) return 800;
  return 450; // 120+ bpm, fast pulse
}

void updatePulse(unsigned long period) {
  float phase = (millis() % period) / (float)period;      // 0..1
  float brightness = (sin(phase * 2 * PI) * 0.5) + 0.5;    // 0..1 sine wave

  uint8_t r = (uint8_t)(((COLOR_PULSE >> 16) & 0xFF) * brightness);
  uint8_t g = (uint8_t)(((COLOR_PULSE >> 8) & 0xFF) * brightness);
  uint8_t b = (uint8_t)((COLOR_PULSE & 0xFF) * brightness);

  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
}

// ================= Slash animation =================

void startSlashAnimation() {
  stateBeforeSlash = currentState;
  currentState = SLASH_ANIM;
  slashStartTime = millis();
}

// Returns true while the animation is still running
bool updateSlashAnimation() {
  unsigned long elapsed = millis() - slashStartTime;
  bool anyLit = false;

  for (int i = 0; i < NUM_LEDS; i++) {
    unsigned long onAt = i * SLASH_STEP_MS;
    unsigned long offAt = onAt + SLASH_TRAIL_MS;

    if (elapsed >= onAt && elapsed < offAt) {
      strip.setPixelColor(i, COLOR_SLASH);
      anyLit = true;
    } else {
      strip.setPixelColor(i, 0);
    }
  }

  // Animation is done once the last LED's off-time has passed
  unsigned long lastOffAt = (NUM_LEDS - 1) * SLASH_STEP_MS + SLASH_TRAIL_MS;
  return elapsed < lastOffAt;
}

// ================= Setup =================

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin(SDA_PIN, SCL_PIN);

  strip.begin();
  strip.show();

  if (!mpuPresent()) {
    Serial.println("MPU6500 not found.");
  } else {
    mpuWake();
    Serial.println("MPU6500 ready.");
  }

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 not found.");
    while (1) delay(1000);
  }
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);
  Serial.println("MAX30102 ready.");
}

// ================= Main loop =================

void loop() {
  long irValue = particleSensor.getIR();
  fingerPresent = irValue > 50000;

  // Beat detection only makes sense while a finger is present
  if (fingerPresent && checkForBeat(irValue)) {
    long delta = millis() - lastBeat;
    lastBeat = millis();
    float bpm = 60.0 / (delta / 1000.0);
    if (bpm > 20 && bpm < 255) {
      rates[rateSpot++] = (byte)bpm;
      rateSpot %= RATE_SIZE;
      int sum = 0;
      for (byte x = 0; x < RATE_SIZE; x++) sum += rates[x];
      beatAvg = sum / RATE_SIZE;
    }
  }

  // Slash detection interrupts whatever state we're in (unless already mid-animation)
  if (currentState != SLASH_ANIM && checkForSlash()) {
    startSlashAnimation();
  }

  // State transitions based on sensor state
  if (currentState != SLASH_ANIM) {
    if (!fingerPresent) {
      currentState = IDLE_OFF;
      beatAvg = 0; // reset so a fresh contact starts back at "searching"
    } else if (beatAvg == 0) {
      currentState = SEARCHING;
    } else {
      currentState = BPM_PULSE;
    }
  }

  // Render current state
  switch (currentState) {
    case IDLE_OFF:
      strip.clear();
      break;
    case SEARCHING:
      updatePulse(2500); // slow "searching" breathing pulse
      break;
    case BPM_PULSE:
      updatePulse(pulsePeriod(beatAvg));
      break;
    case SLASH_ANIM:
      if (!updateSlashAnimation()) {
        currentState = stateBeforeSlash; // animation finished, resume prior state
      }
      break;
  }

  strip.show();
  delay(10); // small loop delay; keep low so animations stay smooth
}
