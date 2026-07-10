# Hardware Design
## Overview
The hardware consists of a rechargable power subsystem, an ESP32 microcontroller, two sensors, and an addressable LED strip.

---
## Main Components
| Component | Purpose |
|--------|---------|
| ESP32-WROOM-32D | Main Controller |
| MAX30102 | Heart rate sensing |
| MPU6500 | Motion sensing |
| SK6812 LEDs | Lighting |
| TP4056 | Battery charging |
| MT3608 | 5V boost converter |
| AMS 1117 | 3.3V regulator |
| LiPo Battery | Portable power |

---
## Power Architecture
```
USB-C
  |
TP4056
  |
LiPo Battery
  |
MT3608 Boost
  |
+5V
  |--- LED Strip
  |--- AMS1117
          |
       3.3V
          |
ESP32
  |--- Sensors
```
---
## Sensor Connections
### MAX30102
- SDA -> ESP32 SDA
- SCL -> ESP32 SCL
- 3.3V
- GND

### MPU6500
- SDA -> ESP32 SDA
- SCL -> ESP32 SCL
- 3.3V
- GND

Both sensors share the same I2C bus.

---
## LED Interface
The SK6812 LED strip is driven using a single GPIO pin.
Recommended protection:
- 330 Ω resistor on data line
- 1000 µF capacitor across 5V input

---
## Design Considerations
- Separate 5V and 3.3V rails
- Common ground throughout system
- Decoupling capacitors near every IC
- Current limiting for LEDs
- Battery charging protection
- Thermal considerations during high LED brightness
