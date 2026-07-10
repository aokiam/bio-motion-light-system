# System Architecture
## Overview
The system consitis of five primary subsystems:
1. Power System
2. Embedded Controller
3. Sensors
4. LED Output
5. Bluetooth Interface

```
Battery
    |
TP4065 Carger
    |
+5V   ------------> LED Strip
    |
AMS1117
    |
+3.3V
    |
ESP32
 |--- MAX30102
 |--- MPU6500
 |--- BLE
 |--- SK6812
```
---
## Data Flow
Heart Rate Sensor -> Sensor Manager -> State Machine -> Animation Engine -> LED Driver -> SK6812 LEDs

The IMU follows the same processing pipeline and contributes additional movement information used by the animation engine.

---
## Communication Interfaces
| Device | Interface |
|-----------|---------|
| MAX30102 | I2C |
| MPU6500 | I2C |
| Mobile App | BLE |
| SK6812 LEDs | Single-wire data|

---
## Timing Requirements
| Task | Frequency |
|-------|-------|
| Heart Rate | 25Hz |
| IMU | 100 Hz |
| LED Refresh | 60 FPS |
| BLE Updates | <100 ms |