# Moon Counter 🌙

A C++ / ESP-IDF based **moon phase tracker** running on a Waveshare 1.69" ESP32 display.  
It shows the current moon phase, local time, and weather information on a tiny always-on desk display.

> Repo: https://github.com/brinae-store/moon_counter

---

## Features

- 🌗 **Live moon phase** calculation
- 🕒 **Auto time sync** via NTP (no manual clock setting)
- 🌡️ **Current temperature & conditions** pulled from a weather API (e.g. OpenWeather)
- 🖥️ **Crisp TFT graphics** on the 1.69" ST7789-based display
- 💾 Asset storage using SPIFFS (images, fonts, etc.)
- 🔄 Periodic refresh so data and phase stay up to date
- ⚙️ Fully configurable Wi-Fi and API settings via ESP-IDF configuration

---

## Hardware

This project was built for:

- **Microcontroller:** Waveshare 1.69" ESP32 display (ESP32-S3 based dev board)
- **Display:** 1.69" ST7789 TFT (built into the board)
- **Power:** USB-C (from your computer, USB brick, or hub)
- **Optional:**
  - 3D-printed stand / enclosure
  - Adhesive feet or magnets if you want it on a metal surface

You can adapt this code to other ESP32 + ST7789 boards with small changes to the pin configuration.

---

## Project Structure

High-level layout:

- `.vscode/` – VS Code / ESP-IDF extension settings
- `build/` – Generated build artifacts (ignored in Git normally)
- `components/` – Custom components (display driver, weather client, moon phase logic, etc.)
- `font/` – Font files for rendering text
- `main/` – Main application code (`main.c` / `main.cpp` and related files)
- `spiffs/` – Assets to be flashed to SPIFFS (icons, images, etc.)
- `CMakeLists.txt` – ESP-IDF CMake project definition
- `sdkconfig*` – ESP-IDF configuration files

---

## Getting Started

### 1. Prerequisites

You’ll need:

- [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/) installed and set up
  - This project targets **ESP32-S3**, so ESP-IDF v5.x is recommended.
- A working Python environment (comes with ESP-IDF tools)
- USB-C cable to connect the Waveshare board to your computer

Make sure you can run:

```bash
idf.py --version
