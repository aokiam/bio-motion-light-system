# Project Proposal
## Project Title
BioMotion Light System
## Author
Allyson Aoki

---
## Overview
The BioMotion Light System is an ESP32-based embedded platform that creates dynamic LED lighting effects based on physiological and motion data. The system monitors the wearer's heart rate using a MAX30102 optical sensor and body movement using an MPU6500 inertial measurement unit (IMU). These inputs are processed in real time to generate responsive lighting animations on an addressable SK6812 LED strip.

A Bluetooth Low Energy (BLE) interface allows users to configure brightness, animation modes, color themes, and sensor sensitivity through a mobile application.

The project combines embedded firmware, power electronics, sensor integration, wireless communication, and wearable system design into a single portable platform.

---
## Objectives
### Functional Objectives
- Read heart rate continually
- Detect user movement
- Generate responsive LED animations
- Support BLE configuration
- Operate from rechargable battery power

### Educational Objectives
- Embedded firmware architecture
- I2C sensor integration
- BLE communication
- Real-time scheduling
- Power management
- Wearable electronics design

--- 
## Motivation
One of my hobbies includes cosplay, and this is planned to be implemented into a homemade cosplay headpiece. Traditional cosplay lighting systems operate generally using predefined animation sequences with limited interaction. This project explores context-aware wearable electronics that react naturally to the user's physical state, producing immersive lighting effects while remaining lightweight and portable.

---
## Expected Deliverables
- Functional embedded firmware
- Working hardware prototype
- BLE companion application
- Hardware documentation
- Wiring diagrams
- Testing documentation
- GitHub repository
