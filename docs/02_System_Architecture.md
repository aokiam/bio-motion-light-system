# System Architecture
## Overview
The system consitis of five primary subsystems:
1. Power System
2. Embedded Controller
3. Sensors
4. LED Output
5. Bluetooth Interface

```
Power Bank
    |
ESP32 3.3V Rail
 |--- MAX30102
 |--- MPU6500
 |--- BLE
 |--- WS2812B
ESP32 VIN (5V) Rail
 |--- LED Strip
```
---
## Data Flow
Heart Rate Sensor -> Sensor Manager -> State Machine -> Animation Engine -> LED Driver -> WS2812B LEDs

The IMU follows the same processing pipeline and contributes additional movement information used by the animation engine.

---
## Communication Interfaces
| Device | Interface |
|-----------|---------|
| MAX30102 | I2C |
| MPU6500 | I2C |
| Mobile App | BLE |
| WS2812B LEDs | Single-wire data|

---
## Timing Requirements
| Task | Frequency |
|-------|-------|
| Heart Rate | 25Hz |
| IMU | 100 Hz |
| LED Refresh | 60 FPS |
| BLE Updates | <100 ms |