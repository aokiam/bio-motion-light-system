# Firmware Architecture
## Overview
The firmware follows a modular architecture that separates hardware drivers from application logic.
```
Main Loop
|
|--- Sensor Manager
|
|--- BLE Manager
| 
|--- Settings Manager
|
|--- State Machine
|
|--- Animation Engine
|
|--- LED Driver
```
## Modules
### Sensor Manager
Responsibilities:
- Read MAX30102
- Read MPU6500
- Filter noise measurements
- Store latest sensor values

---
### BLE Manager
Responsibilities:
- Handle BLE advertising
- Pair with phone
- Receive user settings
- Update configuration

---
### Settings Manager
Stores:
- Brightness
- Color palette
- Motion sensitivity
- Heart-rate sensitivity
- Animation mode

---
### State Machine
Controls operating modes:
- Idle
- Active
- High Motion
- Elevated Heart Rate
- Sleep

Transitions occur based on sensor thresholds

---
### Animation Engine
Uses:
- Heart rate
- Motion data
- User settings

Outputs LED frames at 60 FPS.

---
### LED Driver
Responsibilities:
- Convert animation buffer
- Send pixel data
- Maintain frame timing

--- 
### Main Execution Loop
1. Read sensors
2. Process BLE packets
3. Update state machine
4. Generate animation frame
5. Send LEDs
6. Repeat