# BioMotion Light System
An ESP32-based wearable embedded system that visualizes physiological and motion data using addressable LEDs. The system combines heart-rate sensing, inertial measurement, Bluetooth Low Energy (BLE), and real-time LED animation to create adaptive lighting effects.

## Status
**IN DEVELOPMENT**

Current milestone: Mobile App Integration

## Features
- ESP32-WROOM-32D firmware
- BLE companion app
- Heart-rate reactive LEDs
- Motion-reactive animations
- WS2812B addressable LEDs
- Real-time sensor processing
- Modular hardware architecture

## Hardware
### Core Components
| Component | Purpose |
|-------|-------|
| ESP32-WROOM-32D | Main microcontroller |
| MAX30102 | Optical heart rate sensor |
| MPU6500 | 6-axis IMU |
| WS2812B LEDs | Addressable LED lighting |
| USB-C Power Bank | Portable power for ESP32 |


## Software Architecture
The firmware is organized into independent modules to simplify development and future expansion.
```
Main Loop
|
|--- Sensor Manager
|--- BLE Manager
|--- Settings Manager
|--- State Machine
|--- Animation Engine
|--- LED Driver
```

### Responsibilities
- **Sensor Manager** - Reads heart-rate and IMU data
- **BLE Manager** - Handles communication with the mobile app
- **Settings Manager** - Stores user preferences
- **State Machine** - Controls device operating modes
- **Animation Engine** - Generates LED effects from sensor data
- **LED Driver** - Updates the WS2812B LEDs

## Demo

## BOM
| Component | Quantity | Link |
|-------|-------|-------|
| ESP32-WROOM-32D | 1 | [Amazon](amazon.com/gp/product/B0CR5Y2JVD/ref=ox_sc_saved_title_1?smid=A2Z10KY0342329&psc=1) / [Aliexpress](https://www.aliexpress.us/item/3256808317516268.html?spm=a2g0o.order_list.order_list_main.59.5c6a18023KM8O9&gatewayAdapt=glo2usa)
| MAX30102 | 1 | [Amazon](https://www.amazon.com/AEDIKO-MAX30102-Detection-Concentration-Compatible/dp/B09LQDW27N/ref=sr_1_3?crid=14IW4SWZTLGUF&dib=eyJ2IjoiMSJ9.eq9X1ApZBk_H61_6JV8TSzdohBTfrwtNi6vjeDBPy_7z5e6mcNO-OzU89RsHO6muls6TlS79-huGM4jT9mnlH9mXZR9SmOQnJLYvdYpI26pXSJpOVO_pVn5T86vAPT4k29oRyiW09RhtnfgDXcE2Q-p6ERLBuWCaR-xKStHWBi6b_GECDE8bT2wQ95C7ml1m._d5YRRe2lEyxQVhi-LF8DHrkaMFiSK8_VIG-B8afsZU&dib_tag=se&keywords=max30102&qid=1784246022&s=electronics&sprefix=max30102%2Celectronics%2C191&sr=1-3) / [Aliexpress](https://www.aliexpress.us/item/3256806829092762.html?spm=a2g0o.order_list.order_list_main.41.5c6a18023KM8O9&gatewayAdapt=glo2usa)
| MPU6500 | 1 | [Amazon](https://www.amazon.com/QCCAN-GY-6500-MPU-6500-Gyroscope-Accelerometer/dp/B0B74QXZS8/ref=sr_1_9?crid=1YY01P9OC3KSM&dib=eyJ2IjoiMSJ9.DxBlHobOnDhJrtsCwX5sCMu4lSbi2lNWQJ1mwv0bV39nTUKo5ApkI3P-sIFVp-_2jPe2fvDYTkDUU471jY4juc5riPbA8BYj8UBxPf6u_9q6mRLI6uvgjcihJqhffo345N9sIIeJLB-LnsUOcrJ1r2HSsPGiFgpFO-x1aYpf2vNIfJDghUObZ3F6b-44WjTlIlV1iXZ-1aj_8jban01avXCq0zm__0RMt-QpgXjWfX_AHp3etN4MmEeuV8jWRaPl-9xtYQnR2_Z6gePtrX7-w1jns-k3WExAiYxdDt6sqZI.fFAJHPZaw712TZFTAjXcTp6ayo3mfSOuD16ZGjoiSSA&dib_tag=se&keywords=mpu6500&qid=1784246002&s=electronics&sprefix=mpu6500%2Celectronics%2C238&sr=1-9-catcorr) / [Aliexpress](https://www.aliexpress.us/item/3256807106311918.html?spm=a2g0o.order_list.order_list_main.47.5c6a18023KM8O9&gatewayAdapt=glo2usa)
| WS2812B LEDs | 1 | [Amazon](https://www.amazon.com/dp/B088FJF9XD?ref=ppx_yo2ov_dt_b_fed_asin_title)
| USB-C Power Bank | 1 | [Amazon](https://www.amazon.com/dp/B0C6XK77HJ?ref=ppx_yo2ov_dt_b_fed_asin_title&th=1)
| Capacitors | Various | [Amazon](https://www.amazon.com/dp/B094HRK8QX?th=1)
| Resistors | Various | [Amazon](amazon.com/BOJACK-Values-Resistor-Resistors-Assortment/dp/B08FD1XVL6/ref=sr_1_3?crid=11U68YXQ4GGL9&dib=eyJ2IjoiMSJ9.ZA9QUhgtnvrS43dsRj2R0Cgy41Nr97HX9SyGDpZDWpPWZZPgmlStloE5j-S4aVRzhNzVbHujwhh2rykXNWfSwWtxrdj0LjP2nuBT9WeIb1yrirSbLG2rh23-9O-s8JNNv8sogRjmW4ZHQmak2ZAvcZmQZiUZCB9a84dqvmNuLrD1v7yqO8pJ9xzG38eocl9LCAgt32m8WE6QjvAYi8n43Y5nB8lfalTQB03mP4tsoRA.vj05dTqYUgw8NiO9xexmNOf0Y1FdqwWJqsNqr4UUW60&dib_tag=se&keywords=resistor+kit&qid=1784245899&sprefix=resistor+kit%2Caps%2C369&sr=8-3)
| Perfboard | 2 40x60mm and 1 20x80mm | [Amazon](amazon.com/dp/B0BWCFH57N?ref=ppx_yo2ov_dt_b_fed_asin_title)

## Future Work
- Custom PCB design
- Mobile app expansion
- Multiple synchronized systems
