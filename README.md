# ESP32-S3 WiFi Matrix Clock

A simple and lightweight **ESP32-S3 WiFi Clock** using four MAX7219 8×8 LED matrix modules (8×32 display).

The clock automatically connects to WiFi using **WiFiManager**, synchronizes time from an NTP server, and displays:

- 🕒 Current Time (HH:MM:SS)
- 📅 Current Date (DD/MM/YY)
- 📶 WiFi indicator while connecting
- 🔘 Button to switch between Time and Date

---

# Features

- ESP32-S3 compatible
- Automatic WiFi configuration using WiFiManager
- NTP time synchronization
- Time and Date display
- Automatic WiFi reconnection
- Custom 3×7 pixel font
- No RTC module required

---

# Hardware Required

| Component | Quantity |
|-----------|:--------:|
| ESP32-S3 Development Board | 1 |
| MAX7219 8×8 LED Matrix Module | 4 |
| Push Button | 1 |
| 5V Power Supply (Recommended) | 1 |
| Jumper Wires | As Needed |

---

# Wiring

## MAX7219 → ESP32-S3

| MAX7219 Pin | ESP32-S3 Pin |
|-------------|--------------|
| VCC | 5V |
| GND | GND |
| DIN | GPIO 5 |
| CS | GPIO 6 |
| CLK | GPIO 7 |

## Push Button

| Button Pin | ESP32-S3 Pin |
|------------|--------------|
| One Side | GPIO 45 |
| Other Side | GND |

The firmware uses the ESP32's internal pull-up resistor, so no external resistor is required.

---

# LED Matrix Connection

Connect the four MAX7219 modules in series.

```
ESP32-S3
    │
    ▼
+---------+
| Matrix1 |
+---------+
      │
      ▼
+---------+
| Matrix2 |
+---------+
      │
      ▼
+---------+
| Matrix3 |
+---------+
      │
      ▼
+---------+
| Matrix4 |
+---------+
```

The first MAX7219 module should be connected directly to the ESP32-S3.

---

# Software Requirements

Install the following Arduino libraries:

- SPI
- WiFi
- WiFiManager

WiFiManager can be installed directly from the Arduino Library Manager.

---

# First Setup

1. Upload the firmware to the ESP32-S3.
2. Power on the board.
3. The display will show **wifi**.
4. Connect your phone or computer to the WiFi hotspot:

```
ESP32-Clock-Config
```

5. Open your browser and go to:

```
http://192.168.4.1
```

6. Select your WiFi network.
7. Enter the WiFi password.
8. Save the configuration.

The ESP32 will restart automatically and synchronize the time from the NTP server.

---

# Button Function

| Action | Function |
|--------|----------|
| Press the button | Switch between Time and Date display |

---

# Display Format

### Time

```
HH:MM:SS
```

Example:

```
13:45:28
```

### Date

```
DD/MM/YY
```

Example:

```
27/07/26
```

---

# Time Zone

Default timezone:

```
GMT +7
```

To change the timezone, edit:

```cpp
const long gmtOffset_sec = 25200;
```

---

# Pin Configuration

```cpp
#define DIN_PIN     5
#define CS_PIN      6
#define CLK_PIN     7
#define BUTTON_PIN 45
```

---

# Notes

- Internet access is required for NTP synchronization.
- No external RTC module is needed.
- The clock automatically reconnects if WiFi is disconnected.
- The display shows **wifi** while waiting for a network connection.

---

# License

This project is released under the MIT License.

You are free to use, modify, and distribute this project.

---

Made with ❤️ by **HaiDang**
