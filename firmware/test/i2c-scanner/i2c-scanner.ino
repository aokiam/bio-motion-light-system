/******************************************************************************************** 
Connection test for MPU6500 (IMU) and MAX 30102 (heart rate) on an ESP32-WROOM-32D
via I2C, sharing one bus. Verifies SDA and SCL connections.

Wiring:
  ESP32 3V3  -> 3.3V rail
  ESP32 GND  -> GND rail
  ESP32 GPIO21 (SDA) -> SDA rail
  ESP32 GPIO22 (SCL) -> SCL rail
  5.1k pull-up: SDA rail -> 3.3V
  5.1k pull-up: SCL rail -> 3.3V
  MPU6500: VCC->3.3V, GND->GND, SCL->SCL rail, SDA->SDA rail, AD0->GND (address 0x68)
  MAX30102: VIN->3.3V, GND->GND, SCL->SCL rail, SDA->SDA rail

Author: Allyson Aoki
********************************************************************************************/

#include <Wire.h>

void setup() {
  Wire.begin(21, 22); // SDA, SCL
  Serial.begin(115200);
  delay(1000);
  Serial.println("I2C scanner starting...");
}

void loop() {
  byte count = 0;
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("Found device at 0x");
      Serial.println(addr, HEX);
      count++;
    }
  }
  if (count == 0) Serial.println("No I2C devices found");
  delay(3000);
}