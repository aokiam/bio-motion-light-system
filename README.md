# BioMotion Light System
An ESP32-based wearable embedded system that visualizes physiological and motion data using addressable LEDs. The system combines heart-rate sensing, inertial measurement, Bluetooth Low Energy (BLE), and real-time LED animation to create adaptive lighting effects.

## Status
**IN DEVELOPMENT**

Current milestone: Circuit integration

## Features
- ESP32-WROOM-32D firmware
- BLE companion app
- Heart-rate reactive LEDs
- Motion-reactive animations
- Rechargable LiPo battery
- SK6812 addressable LEDs
- Real-time sensor processing
- Modular hardware architecture

## Hardware
### Core Components
| Component | Purpose |
|-------|-------|
| ESP32-WROOM-32D | Main microcontroller |
| MAX30102 | Optical heart rate sensor |
| MPU6500 | 6-axis IMU |
| SK6812 LEDs | Addressable LED lighting |
| TP4056 | LiPo charging circuit |
| MT3608 | 5V boost converter |
| AMS1117-3.3 | Voltage regulator |
| 3.7V LiPo Battery | Portable power source |


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
- **LED Driver** - Updates the SK6812 LEDs

## Demo

## BOM
| Component | Quantity | Link |
|-------|-------|-------|
| ESP32-WROOM-32D | 1 | [Amazon](amazon.com/gp/product/B0CR5Y2JVD/ref=ox_sc_saved_title_1?smid=A2Z10KY0342329&psc=1) / [Aliexpress](https://www.aliexpress.us/item/3256808317516268.html?spm=a2g0o.order_list.order_list_main.59.5c6a18023KM8O9&gatewayAdapt=glo2usa)
| MAX30102 | 1 | [Amazon](https://www.amazon.com/AEDIKO-MAX30102-Detection-Concentration-Compatible/dp/B09LQDW27N/ref=sr_1_3?crid=14IW4SWZTLGUF&dib=eyJ2IjoiMSJ9.eq9X1ApZBk_H61_6JV8TSzdohBTfrwtNi6vjeDBPy_7z5e6mcNO-OzU89RsHO6muls6TlS79-huGM4jT9mnlH9mXZR9SmOQnJLYvdYpI26pXSJpOVO_pVn5T86vAPT4k29oRyiW09RhtnfgDXcE2Q-p6ERLBuWCaR-xKStHWBi6b_GECDE8bT2wQ95C7ml1m._d5YRRe2lEyxQVhi-LF8DHrkaMFiSK8_VIG-B8afsZU&dib_tag=se&keywords=max30102&qid=1784246022&s=electronics&sprefix=max30102%2Celectronics%2C191&sr=1-3) / [Aliexpress](https://www.aliexpress.us/item/3256806829092762.html?spm=a2g0o.order_list.order_list_main.41.5c6a18023KM8O9&gatewayAdapt=glo2usa)
| MPU6500 | 1 | [Amazon](https://www.amazon.com/QCCAN-GY-6500-MPU-6500-Gyroscope-Accelerometer/dp/B0B74QXZS8/ref=sr_1_9?crid=1YY01P9OC3KSM&dib=eyJ2IjoiMSJ9.DxBlHobOnDhJrtsCwX5sCMu4lSbi2lNWQJ1mwv0bV39nTUKo5ApkI3P-sIFVp-_2jPe2fvDYTkDUU471jY4juc5riPbA8BYj8UBxPf6u_9q6mRLI6uvgjcihJqhffo345N9sIIeJLB-LnsUOcrJ1r2HSsPGiFgpFO-x1aYpf2vNIfJDghUObZ3F6b-44WjTlIlV1iXZ-1aj_8jban01avXCq0zm__0RMt-QpgXjWfX_AHp3etN4MmEeuV8jWRaPl-9xtYQnR2_Z6gePtrX7-w1jns-k3WExAiYxdDt6sqZI.fFAJHPZaw712TZFTAjXcTp6ayo3mfSOuD16ZGjoiSSA&dib_tag=se&keywords=mpu6500&qid=1784246002&s=electronics&sprefix=mpu6500%2Celectronics%2C238&sr=1-9-catcorr) / [Aliexpress](https://www.aliexpress.us/item/3256807106311918.html?spm=a2g0o.order_list.order_list_main.47.5c6a18023KM8O9&gatewayAdapt=glo2usa)
| SK6812 LEDs | 1 | [Amazon](https://www.amazon.com/dp/B0FBG8TL3P/ref=twister_B0F6MLBZR2?_encoding=UTF8&th=1) / [Aliexpress](https://www.aliexpress.us/item/3256807677321116.html?spm=a2g0o.order_list.order_list_main.17.5c6a18023KM8O9&gatewayAdapt=glo2usa)
| LiPo Battery | 1 | [Amazon](https://www.amazon.com/dp/B0FH7G1WPG?ref=ppx_yo2ov_dt_b_fed_asin_title&th=1)
| MT3608 | 1 | [Amazon](https://www.amazon.com/Ferwooh-Converter-Adjustable-Voltage-Regulator/dp/B0D17PHDSD/ref=sr_1_3?crid=27QER05RZWEXI&dib=eyJ2IjoiMSJ9.NP_yxp2ePIoUSBiPrZYdUG1TL5JxLsDuj_HRebKhGZ7guztNZcTMjtL1jwdiX4jQSdRn75DsrMTqDgxfn1HxW4NZVYmhOCN5scJDY67-MfHrqx8Iltd1DUpBPAx2ZtWvfE1KTKhDXJBl2rNzGSmxoxj-4Us2Q5fmhzgkvzFMCB3T4U982mwQneKijrM6dIvmWVv39babFqqBP7OX8BzUJGYSQupm64hyN2x4du39Q0k.7CBOvPnXGglYJsTRRTJKEHxvn2c0-VRgVH7wWRDqFaw&dib_tag=se&keywords=mt3608&qid=1784246240&sprefix=mt3608%2Caps%2C248&sr=8-3) / [Aliexpress](https://www.aliexpress.us/item/3256807143629614.html?spm=a2g0o.order_list.order_list_main.35.5c6a18023KM8O9&gatewayAdapt=glo2usa)
| TP4056 | 1 | [Amazon](https://www.amazon.com/HiLetgo-Lithium-Charging-Protection-Functions/dp/B07PKND8KG/ref=sr_1_4?crid=1JYM66N4U1BZ&dib=eyJ2IjoiMSJ9.-UiBtZJmSZvxE3u_ela8Feh5pMzy8UH-dxhGQSHejlrzdXQCJylwlmlo9us58JDQN78PsUY7IQ73nD5tfmrTJtHesreAZ3aNM6db6wFCRCqNBj6mxFklUYK-5zdi_pAxX9o4GhzEfjBmq-cZ2AnFKQdvwtY5luakVMcgOw3Ca6WH_ZrKgPIvuF8H4dBNl5qOOIFyp7PQQjJhDjgV1rKbQ_ljVuRXVs-KZqQdPUcIkSY.bRjMTsOpS7qt-I05dgpt6ZosMYCJ4Pwkv0M3ZvG05M0&dib_tag=se&keywords=tp4056&qid=1784246297&sprefix=tp405%2Caps%2C272&sr=8-4) / [Aliexpress](https://www.aliexpress.us/item/3256810410652428.html?spm=a2g0o.order_list.order_list_main.29.5c6a18023KM8O9&gatewayAdapt=glo2usa)
| AMS1117-3.3 | 1 | [Amazon](https://www.amazon.com/Anmbest-AMS1117-3-3-4-75V-12V-Voltage-Regulator/dp/B07CP4P5XJ/ref=sr_1_3?crid=2USTX3C2UFBTT&dib=eyJ2IjoiMSJ9.rp669YubkiFBdkg798DmHPLQHQW1DZR_k89X9TQr71Z8q4Zjpz8BQ3ANKsl2wCKTIakDyy1r07SnQftOkDfsMtRE2tpfy5tKtXTXa9QCdKPz_8JNn-GQBBdiu7Vka8gfaCTClzju2CbNWEDoEDG_HdCfCpnITsNvJCPbpwEK875e9bWk1PL5XFqTLJXCz0Q__VPbAFicLn-tZoOnX64mb1erqoRguc9mOTZA-v6An4s.kUu5J6kx3dP_XbwSl6Kx5GhhRa1A5hR3Xvp7qB_TLeY&dib_tag=se&keywords=ams1117-3.3+module&qid=1784246325&sprefix=ams1117%2Caps%2C243&sr=8-3) / [Aliexpress](https://www.aliexpress.us/item/3256807606391602.html?spm=a2g0o.order_list.order_list_main.5.5c6a18023KM8O9&gatewayAdapt=glo2usa)
| Capasitors | Various | [Amazon](https://www.amazon.com/dp/B094HRK8QX?th=1)
| Resistors | Various | [Amazon](amazon.com/BOJACK-Values-Resistor-Resistors-Assortment/dp/B08FD1XVL6/ref=sr_1_3?crid=11U68YXQ4GGL9&dib=eyJ2IjoiMSJ9.ZA9QUhgtnvrS43dsRj2R0Cgy41Nr97HX9SyGDpZDWpPWZZPgmlStloE5j-S4aVRzhNzVbHujwhh2rykXNWfSwWtxrdj0LjP2nuBT9WeIb1yrirSbLG2rh23-9O-s8JNNv8sogRjmW4ZHQmak2ZAvcZmQZiUZCB9a84dqvmNuLrD1v7yqO8pJ9xzG38eocl9LCAgt32m8WE6QjvAYi8n43Y5nB8lfalTQB03mP4tsoRA.vj05dTqYUgw8NiO9xexmNOf0Y1FdqwWJqsNqr4UUW60&dib_tag=se&keywords=resistor+kit&qid=1784245899&sprefix=resistor+kit%2Caps%2C369&sr=8-3)
| Perfboard | 2 40x60mm and 1 20x80mm | [Amazon](amazon.com/dp/B0BWCFH57N?ref=ppx_yo2ov_dt_b_fed_asin_title)

## Future Work
- Custom PCB design
- Mobile app expansion
- Battery level monitoring
- Multiple synchronized systems