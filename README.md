# Moon Counter 🌙

A C++ / ESP-IDF based **moon phase tracker** running on a Waveshare
1.69" ESP32 display.\
It shows the current moon phase, local time, and weather information on
a tiny always-on desk display.

> Repo: https://github.com/brinae-store/moon_counter

------------------------------------------------------------------------

## Features

-   🌗 **Live moon phase** calculation
-   🕒 **Automatic NTP time sync**
-   🌡️ **Live temperature & weather conditions** (OpenWeather API)
-   🖥️ **Crisp TFT graphics** rendered on an ST7789 240×280 display
-   💾 **SPIFFS asset storage** for images, icons, fonts
-   🔄 Periodic background refresh (time, moon, weather)
-   ⚙️ ESP-IDF configuration for Wi-Fi + API key
-   🔌 USB-C powered desk companion device

------------------------------------------------------------------------

## Hardware

This project is designed for:

-   **Microcontroller:** Waveshare 1.69" ESP32 Display (ESP32-S3)
-   **Display:** ST7789 1.69" TFT (240×280)
-   **Storage:** SPIFFS partition for icons + assets
-   **Connectivity:** Wi-Fi 2.4 GHz
-   **Power:** USB-C cable

You can adapt this code to other ESP32 boards with small pin
modifications.

------------------------------------------------------------------------

## Project Structure

    moon_counter/
    ├── .vscode/               # VS Code ESP-IDF config
    ├── components/            # Custom components (display, weather, moon logic)
    ├── font/                  # Font assets for rendering text
    ├── main/                  # Entry point, app logic, rendering code
    ├── spiffs/                # Images + assets flashed to SPIFFS
    ├── CMakeLists.txt         # ESP-IDF build config
    ├── sdkconfig              # Auto-generated ESP-IDF settings
    └── README.md              # This file

------------------------------------------------------------------------

## Getting Started

### 1. Prerequisites

You must have:

-   **ESP-IDF** installed (recommended v5.x)
-   Python installed (included with ESP-IDF tools)
-   A USB-C cable
-   A valid **OpenWeather API key**

Check ESP-IDF availability:

``` bash
idf.py --version
```

------------------------------------------------------------------------

### 2. Clone the Repository

``` bash
git clone https://github.com/brinae-store/moon_counter.git
cd moon_counter
```

Set the correct target:

``` bash
idf.py set-target esp32s3
```

------------------------------------------------------------------------

### 3. Configure Wi-Fi + API Keys

Run:

``` bash
idf.py menuconfig
```

Then configure:

-   **Wi-Fi SSID**
-   **Wi-Fi Password**
-   **OpenWeather API Key**
-   **Location variables** (if applicable)

If the project doesn't expose these yet, set them directly inside the
configuration header in `main` or `components/openweather`.

------------------------------------------------------------------------

### 4. Build, Flash, and Monitor

Connect the device via USB-C, then run:

``` bash
idf.py build
idf.py flash
idf.py monitor
```

You should see logs such as:

    Weather Updated | Temp: 31°F | Moon Phase: 0.72
    Time synced
    Display initialized

Exit the monitor with **Ctrl+\]**.

------------------------------------------------------------------------

## Usage

After flashing:

1.  The ESP32 connects to your Wi-Fi.
2.  Time syncs automatically via NTP.
3.  OpenWeather API fetches the current conditions.
4.  The display renders:
    -   Current moon phase
    -   Local time
    -   Temperature + weather icon
5.  The device refreshes periodically.

Just leave it plugged in --- it's designed as a persistent
desk/nightstand display.

------------------------------------------------------------------------

## Roadmap

-   [x] Live moon phase tracking\
-   [x] Time + weather integration\
-   [ ] Show next full moon / new moon countdown\
-   [ ] Alternate UI layouts / themes\
-   [ ] Add battery support\
-   [ ] Add BLE configuration mode\
-   [ ] Add animations for moon transitions

------------------------------------------------------------------------

## License

This project is licensed under the **MIT License**.\
Commercial and personal use allowed. Modify freely.

------------------------------------------------------------------------
