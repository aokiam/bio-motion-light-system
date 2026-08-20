#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// PIN config
#define SDA_PIN 21
#define SCL_PIN 22
#define LED_PIN 26
#define NUM_LEDS 25

const int MPU_ADDR = 0x68;
MAX30105 particleSensor;
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// BLE UUIDS -----------  ***match with App.jsx***
#define SERVICE_UUID            "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define BRIGHTNESS_CHAR_UUID    "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define ANIM_MODE_CHAR_UUID     "0a3b0f1e-6b0a-4a6a-9a3a-3e6b1a2c4d5e"
#define HEART_RATE_CHAR_UUID    "6b3f1a4c-2d5e-4f6a-8b9c-1a2b3c4d5e6f"
#define COLOR_CHAR_UUID         "9c1e2f3a-4b5c-4d6e-8f7a-1b2c3d4e5f6a"
#define HR_INPUT_CHAR_UUID      "2d3e4f5a-6b7c-4d8e-9f0a-1b2c3d4e5f6b"
#define MOTION_INPUT_CHAR_UUID  "5a6b7c8d-9e0f-4a1b-8c2d-3e4f5a6b7c8d"

BLECharacteristic *brightnessChar;
BLECharacteristic *animModeChar;
BLECharacteristic *heartRateChar;
BLECharacteristic *colorChar;
BLECharacteristic *hrInputChar;
BLECharacteristic *motionInputChar;

bool deviceConnected = false;

// live settings; what the BLE callbacks actually change
enum AnimMode { MODE_PULSE, MODE_SLASH, MODE_DYNAMIC };
AnimMode currentAnimMode = MODE_DYNAMIC;

uint8_t globalBrightness = 128;
uint32_t activeColor = 0x00BFFF;
bool heartRateInputEnabled = true;
bool motionInputEnabled = true;

// heart rate detection state
const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
int beatAvg = 0;
bool fingerPresent = false;

// LED state machine
enum LedState { IDLE_OFF, SEARCHING, BPM_PULSE, SLASH_ANIM };
LedState currentState = IDLE_OFF;
LedState stateBeforeSlash = IDLE_OFF;

// slash detection
float GYRO_SLASH_THRESHOLD = 300.0; //  units = deg/s
unsigned long lastSlashTime = 0;
const unsigned long SLASH_COOLDOWN = 800;

unsigned long slashStartTime = 0;
const unsigned long SLASH_STEP_MS = 40;
const unsigned long SLASH_TRAIL_MS = 1000;

// ----------------- MPU6500 helpers ------------------
bool mpuPresent() {
  Wire.beingTransmission(MPU_ADDR);
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
  Wire.write(0x43);
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

// ----------------- PULSE animation ------------------
unsigned long pulsePeriod(int bpm) {
  if (bpm < 60)  return 1800;
  if (bpm < 90)  return 1200;
  if (bpm < 120) return 800;
  return 450;
}

// Applies globalBrightness as a scale on top of a base color, so the
// brightness slider affects every animation the same way.
uint32_t scaledColor(uint32_t color, float scale) {
  uint8_t r = (uint8_t)(((color >> 16) & 0xFF) * scale * (globalBrightness / 255.0));
  uint8_t g = (uint8_t)(((color >> 8) & 0xFF) * scale * (globalBrightness / 255.0));
  uint8_t b = (uint8_t)((color & 0xFF) * scale * (globalBrightness / 255.0));
  return strip.Color(r, g, b);
}

void updatePulse(unsigned long period) {
  float phase = (millis() % period) / (float)period;
  float brightness = (sin(phase * 2 * PI) * 0.5) + 0.5;
  uint32_t c = scaledColor(activeColor, brightness);
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, c);
  }
}

// ----------------- SLASH animation ------------------
void startSlashAnimation() {
  stateBeforeSlash = currentState;
  currentState = SLASH_ANIM;
  slashStartTime = millis();
}

bool updateSlashAnimation() {
  unsigned long elapsed = millis() - slashStartTime;
  uint32_t c = scaledColor(activeColor, 1.0);

  for (int i = 0; i < NUM_LEDS; i++) {
    unsigned long onAt = i * SLASH_STEP_MS;
    unsigned long offAt = onAt + SLASH_TRAIL_MS;
    if (elapsed >= onAt && elapsed < offAt) {
      strip.setPixelColor(i, c);
    } else {
      strip.setPixelColor(i, 0);
    }
  }

  unsigned long lastOffAt = (NUM_LEDS - 1) * SLASH_STEP_MS + SLASH_TRAIL_MS;
  return elapsed < lastOffAt;
}

// ----------------- BLE callbacks ------------------
class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *server) {
    deviceConnected = true;
    Serial.println("Phone connected.");
  }
  void onDisconnect(BLEServer *server) {
    deviceConnected = false;
    Serial.println("Phone disconnected, restarting advertising...");
    BLEDevice::startAdvertising(); // so it can be found again
  }
};

// Called whenever the phone writes to the brightness characteristic
class BrightnessCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) {
    String value = characteristic->getValue();
    if (value.length() > 0) {
      uint8_t brightness = (uint8_t)value[0];
      Serial.print("Brightness set to: ");
      Serial.println(brightness);
      // TODO: apply it to actual LED strip
    }
  }
};

// Called whenever the phone writes to the animation mode characteristic
class AnimModeCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) {
    String value = characteristic->getValue();
    if (value.length() > 0) {
      Serial.print("[ANIMATION] set to: ");
      Serial.println(value);
      
      if (value == "pulse") {
        currentAnimMode = MODE_PULSE;
      } else if (value == "slash") {
        currentAnimMode = MODE_SLASH;
      } else if (value == "dynamic") {
        currentAnimMode = MODE_DYNAMIC;
      } else {
        Serial.println("[ANIMATION] unrecognized value, ignoring.");
      }
    }
  }
};

class ColorCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) {
    String value = characteristic->getValue();
    if (value.length() >= 3) {
      uint8_t r = (uint8_t)value[0];
      uint8_t g = (uint8_t)value[1];
      uint8_t b = (uint8_t)value[2];
      activeColor = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
      Serial.print("[COLOR] Set to R:");
      Serial.print(r);
      Serial.print(" G:");
      Serial.print(g);
      Serial.print(" B:");
      Serial.println(b);
    } else {
      Serial.println("[COLOR] Received malformed value (expected 3 bytes), ignoring.");
    }
  }
};

class HRInputCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) {
    String value = characteristic->getValue();
    if (value.length() > 0) {
      heartRateInputEnabled = (uint8_t)value[0] != 0;
      Serial.print("[HEART RATE INPUT] ");
      Serial.println(heartRateInputEnabled ? "Enabled" : "Disabled");
    }
  }
};

class MotionInputCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) {
    String value = characteristic->getValue();
    if (value.length() > 0) {
      motionInputEnabled = (uint8_t)value[0] != 0;
      Serial.print("[MOTION INPUT] ");
      Serial.println(motionInputEnabled ? "Enabled" : "Disabled");
    }
  }
};

void setup() {
  Serial.begin(9600);
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
  } else {
    particleSensor.setup();
    particleSensor.setPulseAmplitudeRed(0x0A);
    particleSensor.setPulseAmplitudeGreen(0);
    Serial.println("MAX30102 ready.");
  }

  Serial.println();
  Serial.println("Cosplay Prop BLE server starting");

  BLEDevice::init("CosplayProp");
  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new ServerCallbacks());

  BLEService *service = server->createService(SERVICE_UUID);

  brightnessChar = service->createCharacteristic(
    BRIGHTNESS_CHAR_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  brightnessChar->setCallbacks(new BrightnessCallbacks());
  brightnessChar->setValue(&globalBrightness, 1);

  animModeChar = service->createCharacteristic(
    ANIM_MODE_CHAR_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  animModeChar->setCallbacks(new AnimModeCallbacks());
  animModeChar->setValue("pulse");

  heartRateChar = service->createCharacteristic(
    HEART_RATE_CHAR_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  heartRateChar->addDescriptor(new BLE2902());
  uint8_t initialBpm = 0;
  heartRateChar->setValue(&initialBpm, 1);

  colorChar = service->createCharacteristic(
    COLOR_CHAR_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  colorChar->setCallbacks(new ColorCallbacks());
  uint8_t initialColor[3] = {0, 80, 255};
  colorChar->setValue(initialColor, 3);

  hrInputChar = service->createCharacteristic(
    HR_INPUT_CHAR_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  hrInputChar->setCallbacks(new HRInputCallbacks());
  uint8_t hrInputDefault = 1;
  hrInputChar->setValue(&hrInputDefault, 1);

  motionInputChar = service->createCharacteristic(
    MOTION_INPUT_CHAR_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  motionInputChar->setCallbacks(new MotionInputCallbacks());
  uint8_t motionInputDefault = 1;
  motionInputChar->setValue(&motionInputDefault, 1);

  service->start();

  BLEAdvertising *advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  advertising->setScanResponse(true);
  BLEDevice::startAdvertising();

  Serial.println("BLE advertising started. Look for 'CosplayProp'.");
  Serial.println();
}

void loop() {
  bool wantsHeartRate = (currentAnimMode == MODE_PULSE || currentAnimMode == MODE_DYNAMIC) && heartRateInputEnabled;
  bool wantsMotion = (currentAnimMode == MODE_SLASH || currentAnimMode == MODE_DYNAMIC) && motionInputEnabled;

  // --- Heart rate sensing, only when this mode/toggle combination wants it ---
  if (wantsHeartRate) {
    long irValue = particleSensor.getIR();
    fingerPresent = irValue > 50000;

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

    // Push the current BPM to the phone once per second, same as before
    static unsigned long lastNotify = 0;
    if (deviceConnected && millis() - lastNotify > 1000) {
      lastNotify = millis();
      uint8_t bpmByte = (uint8_t)beatAvg;
      heartRateChar->setValue(&bpmByte, 1);
      heartRateChar->notify();
    }
  } else {
    fingerPresent = false;
    beatAvg = 0;
  }

  // --- Motion sensing, only when this mode/toggle combination wants it ---
  bool slashTriggered = false;
  if (wantsMotion && currentState != SLASH_ANIM) {
    slashTriggered = checkForSlash();
  }
  if (slashTriggered) {
    startSlashAnimation();
  }

  // --- State transitions, skipped while a slash animation is actively playing ---
  if (currentState != SLASH_ANIM) {
    if (currentAnimMode == MODE_SLASH) {
      // Slash-only mode has no baseline animation - just idle off between triggers
      currentState = IDLE_OFF;
    } else {
      // Pulse and Dynamic modes both use the heart-rate baseline
      if (!fingerPresent) {
        currentState = IDLE_OFF;
      } else if (beatAvg == 0) {
        currentState = SEARCHING;
      } else {
        currentState = BPM_PULSE;
      }
    }
  }

  // --- Render ---
  switch (currentState) {
    case IDLE_OFF:
      strip.clear();
      break;
    case SEARCHING:
      updatePulse(2500);
      break;
    case BPM_PULSE:
      updatePulse(pulsePeriod(beatAvg));
      break;
    case SLASH_ANIM:
      if (!updateSlashAnimation()) {
        currentState = stateBeforeSlash;
      }
      break;
  }

  strip.show();
  delay(10);
}
