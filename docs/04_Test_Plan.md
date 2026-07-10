# Test Plan
## Objectives
Verify that each subsystem functions independently before validating complete system integration.

---
# Unit Testing
## Heart Rate Sensor
### Procedure
Measure heart rate while resting and compare with a commercial smartwatch

**Expected error:** ±5 BPM

**Pass Criteria:** Measurements remain within tolerance

---
## IMU
Test Cases:
- Stationary
- Walking
- Quick slash
- Quick rotation

Verify acceleration and angular velocity change appropriately.

---
## LED Driver
Verify:
- All LEDs illuminate
- No flickering
- Correct colors
- Smooth animation

---
## BLE
Measure:
- Connection time
- Response latency
- Packet reliability

Target:
Settings update within 100ms.

---
# Integration Testing
Verify:

Heart rate changes -> Animation changes

Movement -> Animation changes

BLE command -> Immediate LED response

---
# Battery Testing
Measure:
- Runtime at 25%
- Runtime at 50%
- Runtime at 100%

Monitor battery voltage throughout discharge.

---
# Stress Testing
Operate continuously for:
- 30 minutes
- 1 hour
- 2 hours

Verify:
- No crashes
- Stable BLE communication
- No overheating
- Consistent sensor readings

---
# Acceptance criteria
The project is considered complete if:
- Heart rate is detected reliably
- Motion is detected accurately
- BLE communication remains stable
- LED animations remain smooth
- Battery operates safely
- System run continuously for at least two hours