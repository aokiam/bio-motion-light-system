# Hardware Design
## Overview
The hardware consists of a rechargable power bank, an ESP32 microcontroller, two sensors, and an addressable LED strip.

---
## Main Components
| Component | Purpose |
|--------|---------|
| ESP32-WROOM-32D | Main Controller |
| MAX30102 | Heart rate sensing |
| MPU6500 | Motion sensing |
| WS2812B LEDs | Lighting |
<<<<<<< HEAD
| Power Bank | Portable power |
=======
| TP4056 | Battery charging |
| MT3608 | 5V boost converter |
| AMS 1117 | 3.3V regulator |
| LiPo Battery | Portable power |
>>>>>>> 4d88ec08d6901f317757a8ecc8b70e3ebece8073

---
## Power Architecture
```
USB-C Power Bank
  |
ESP32 USB-C Port
  | +5V
     |----- LED Strip
  | +3.3V
     |----- Sensors
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
The WS2812B LED strip is driven using a single GPIO pin.
<<<<<<< HEAD
=======
Recommended protection:
- 330 Ω resistor on data line
- 1000 µF capacitor across 5V input
>>>>>>> 4d88ec08d6901f317757a8ecc8b70e3ebece8073

---
## Design Considerations
- Separate 5V and 3.3V rails
- Common ground throughout system
- Decoupling capacitors near every IC
- Thermal considerations during high LED brightness
