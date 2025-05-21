# Track Monitoring System

This project involves building a hardware system using **Arduino and multiple sensors** to measure **track gauge** and **cross-level (rail cant)**. All measurement data is logged to an **SD card** for analysis and inspection.

---

## Overview

### Features

- Measures:
  - **Track gauge**: Distance between inner faces of two rails
  - **Cross-level**: Vertical level difference between two rails
- Real-time measurement using interfaced sensors
- Logs data to an SD card in a text file
- Portable and suitable for on-site track inspection

---

## Hardware Components

- **Arduino Nano**
- **Displacement sensor and Accelerometer** (for gauge & level)
- **SD card module**
- **MicroSD card (FAT32 formatted)**
- **LCD / OLED display** for real-time viewing

> Note: Sensor types depend on the design approach and precision required.

---

## How It Works

- The Arduino reads analog values from two sensors.
- Track gauge is calculated as the distance between sensors mounted across the rail gauge.
- Cross-level is calculated using the tilt measured by the accelerometer.
- Data is time-stamped and saved to the SD card as a '.txt' file.

---

## Data Logging Format

Saved as `data.txt` on the SD card
