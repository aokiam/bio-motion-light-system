/******************************************************************************************** 
Combined connection + functionality test for MPU6500 (IMU) and MAX30102 (heart rate)
on an ESP32-WROOM-32D, sharing one I2C bus.

Wiring:
  ESP32 3V3  -> 3.3V rail
  ESP32 GND  -> GND rail
  ESP32 GPIO21 (SDA) -> SDA rail
  ESP32 GPIO22 (SCL) -> SCL rail
  5.1k pull-up: SDA rail -> 3.3V
  5.1k pull-up: SCL rail -> 3.3V
  MPU6500: VCC->3.3V, GND->GND, SCL->SCL rail, SDA->SDA rail, AD0->GND (address 0x68)
  MAX30102: VIN->3.3V, GND->GND, SCL->SCL rail, SDA->SDA rail

Required library (install via Library Manager):
  "SparkFun MAX3010x Pulse and Proximity Sensor Library"

Author: Allyson Aoki
********************************************************************************************/

#include <Wire.h>
#include "MAX30105.h"

#define SDA_PIN 21
#define SCL_PIN 22

const int MPU_ADDR = 0x68;
MAX30105 particleSensor;

bool mpuOK = false;
bool maxOK = false;

// ---------- MPU6500 helpers ----------

bool mpuPresent() {
  Wire.beginTransmission(MPU_ADDR);
  return (Wire.endTransmission() == 0);
}

void mpuWake() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // PWR_MGMT_1
  Wire.write(0x00); // clear sleep bit
  Wire.endTransmission();
}

void mpuRead(float &axg, float &ayg, float &azg, float &gxd, float &gyd, float &gzd) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // ACCEL_XOUT_H
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);

  int16_t ax = Wire.read() << 8 | Wire.read();
  int16_t ay = Wire.read() << 8 | Wire.read();
  int16_t az = Wire.read() << 8 | Wire.read();
  Wire.read(); Wire.read(); // discard temperature bytes
  int16_t gx = Wire.read() << 8 | Wire.read();
  int16_t gy = Wire.read() << 8 | Wire.read();
  int16_t gz = Wire.read() << 8 | Wire.read();

  axg = ax / 16384.0; ayg = ay / 16384.0; azg = az / 16384.0;
  gxd = gx / 131.0;   gyd = gy / 131.0;   gzd = gz / 131.0;
}

// ---------- Setup ----------

void setup() {
  Serial.begin(115200);
  delay(1500); // give Serial Monitor time to connect
  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.println();
  Serial.println("===== Stage 1: Connection test =====");

  // MPU6500 connection check
  Serial.print("MPU6500 at 0x68 ... ");
  mpuOK = mpuPresent();
  Serial.println(mpuOK ? "FOUND" : "NOT FOUND");
  if (mpuOK) mpuWake();

  // MAX30102 connection check
  Serial.print("MAX30102 ... ");
  maxOK = particleSensor.begin(Wire, I2C_SPEED_FAST);
  Serial.println(maxOK ? "FOUND" : "NOT FOUND");

  if (!mpuOK || !maxOK) {
    Serial.println();
    Serial.println("One or both sensors did not respond. Check:");
    Serial.println(" - SDA/SCL not swapped, correct GPIO pins in code");
    Serial.println(" - 4.7k pull-ups present on SDA and SCL");
    Serial.println(" - VCC/GND wired to every device, breadboard rows making contact");
    Serial.println(" - MPU6500 AD0 tied to GND");
    Serial.println("Halting. Fix wiring and reset the board.");
    while (1) delay(1000);
  }

  Serial.println();
  Serial.println("Both sensors found. Configuring MAX30102...");
  particleSensor.setup(0x1F, 4, 2, 100, 411, 4096); // brightness, avg, mode(Red+IR), sample rate, pulse width, ADC range

  Serial.println();
  Serial.println("===== Stage 2: Functionality test =====");
  Serial.println("Move the board to see accel/gyro change.");
  Serial.println("Rest a fingertip on the MAX30102 to see IR jump.");
  Serial.println();
}

// ---------- Loop ----------

void loop() {
  float axg, ayg, azg, gxd, gyd, gzd;
  mpuRead(axg, ayg, azg, gxd, gyd, gzd);

  long irValue = particleSensor.getIR();
  long redValue = particleSensor.getRed();

  Serial.print("Accel g(x,y,z): ");
  Serial.print(axg, 2); Serial.print(","); Serial.print(ayg, 2); Serial.print(","); Serial.print(azg, 2);

  Serial.print("  Gyro dps(x,y,z): ");
  Serial.print(gxd, 1); Serial.print(","); Serial.print(gyd, 1); Serial.print(","); Serial.print(gzd, 1);

  Serial.print("  IR: "); Serial.print(irValue);
  Serial.print(" Red: "); Serial.print(redValue);
  Serial.println(irValue > 50000 ? "  [finger detected]" : "");

  delay(200);
}

// Expected
// - Stage 1 prints "FOUND" for both sensors and moves on automatically.
// - Stage 2 streams live data. At rest, flat on a table, az should read
//   close to 1.00g and gyro values should sit near 0. Tilting or rotating
//   the board should visibly change the accel/gyro numbers.
// - With no finger on the MAX30102, IR stays low. Resting a fingertip on
//   it should push IR above 50000 and show "[finger detected]".
