#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <time.h>

#define DIN_PIN    5
#define CS_PIN     6
#define CLK_PIN    7
#define BUTTON_PIN 45

#define CMD_DIGIT0      1
#define CMD_DECODEMODE  9
#define CMD_INTENSITY   10
#define CMD_SCANLIMIT   11
#define CMD_SHUTDOWN    12
#define CMD_DISPLAYTEST 15

const uint8_t FONT_NUMBERS[10][3] = {
  {0x7F, 0x41, 0x7F},
  {0x00, 0x7F, 0x00},
  {0x79, 0x49, 0x4F},
  {0x49, 0x49, 0x7F},
  {0x1F, 0x10, 0x7F},
  {0x4F, 0x49, 0x79},
  {0x7F, 0x49, 0x79},
  {0x01, 0x01, 0x7F},
  {0x7F, 0x49, 0x7F},
  {0x4F, 0x49, 0x7F}
};

class Matrix8x32 {
private:
  uint8_t buffer[32];
  const int num = 4;

  void writeAll(uint8_t cmd, uint8_t val) {
    digitalWrite(CS_PIN, LOW);
    for (int i = 0; i < num; i++) {
      SPI.transfer(cmd);
      SPI.transfer(val);
    }
    digitalWrite(CS_PIN, HIGH);
  }

public:
  void init() {
    pinMode(CS_PIN, OUTPUT);
    digitalWrite(CS_PIN, HIGH);

    SPI.begin(CLK_PIN, -1, DIN_PIN, CS_PIN);
    SPI.setFrequency(10000000);

    writeAll(CMD_SHUTDOWN, 0);
    writeAll(CMD_DISPLAYTEST, 0);
    writeAll(CMD_SCANLIMIT, 7);
    writeAll(CMD_DECODEMODE, 0);
    writeAll(CMD_SHUTDOWN, 1);
    writeAll(CMD_INTENSITY, 8);
    clear();
  }

  void clear() {
    memset(buffer, 0, 32);
    show();
  }

  void show() {
    for (int i = 0; i < 8; i++) {
      digitalWrite(CS_PIN, LOW);
      for (int dev = 0; dev < num; dev++) {
        SPI.transfer(CMD_DIGIT0 + i);
        SPI.transfer(buffer[(dev * 8) + i]);
      }
      digitalWrite(CS_PIN, HIGH);
    }
  }

  void setPixel(int x, int y, bool v) {
    int targetX = 31 - x;
    int targetY = 7 - y;

    if (targetX >= 0 && targetX < 32 && targetY >= 0 && targetY < 8) {
      int dev = 3 - (targetX / 8);
      int col = targetX % 8;

      if (v) buffer[(dev * 8) + (7 - targetY)] |= (1 << col);
      else buffer[(dev * 8) + (7 - targetY)] &= ~(1 << col);
    }
  }

  void drawDigit(int x, int digit) {
    if (digit < 0 || digit > 9) return;

    for (int c = 0; c < 3; c++) {
      uint8_t line = FONT_NUMBERS[digit][c];
      for (int r = 0; r < 7; r++) {
        if ((line >> r) & 1) setPixel(x + c, r + 1, true);
      }
    }
  }

  void drawColon(int x) {
    setPixel(x, 2, true);
    setPixel(x, 5, true);
  }

  void drawSlash(int x) {
    setPixel(x, 6, true);
    setPixel(x + 1, 4, true);
    setPixel(x + 1, 3, true);
    setPixel(x + 2, 1, true);
  }

  void drawWifi() {
    memset(buffer, 0, 32);

    setPixel(0, 1, true);
    setPixel(0, 2, true);
    setPixel(0, 3, true);
    setPixel(0, 4, true);
    setPixel(0, 5, true);
    setPixel(1, 6, true);
    setPixel(2, 3, true);
    setPixel(2, 4, true);
    setPixel(2, 5, true);
    setPixel(3, 6, true);
    setPixel(4, 1, true);
    setPixel(4, 2, true);
    setPixel(4, 3, true);
    setPixel(4, 4, true);
    setPixel(4, 5, true);

    setPixel(7, 0, true);
    setPixel(8, 0, true);
    setPixel(7, 2, true);
    setPixel(8, 2, true);
    setPixel(7, 3, true);
    setPixel(8, 3, true);
    setPixel(7, 4, true);
    setPixel(8, 4, true);
    setPixel(7, 5, true);
    setPixel(8, 5, true);
    setPixel(7, 6, true);
    setPixel(8, 6, true);

    setPixel(12, 0, true);
    setPixel(13, 0, true);
    setPixel(14, 0, true);
    setPixel(12, 1, true);
    setPixel(11, 2, true);
    setPixel(12, 2, true);
    setPixel(13, 2, true);
    setPixel(14, 2, true);
    setPixel(12, 3, true);
    setPixel(12, 4, true);
    setPixel(12, 5, true);
    setPixel(12, 6, true);

    setPixel(17, 0, true);
    setPixel(18, 0, true);
    setPixel(17, 2, true);
    setPixel(18, 2, true);
    setPixel(17, 3, true);
    setPixel(18, 3, true);
    setPixel(17, 4, true);
    setPixel(18, 4, true);
    setPixel(17, 5, true);
    setPixel(18, 5, true);
    setPixel(17, 6, true);
    setPixel(18, 6, true);

    show();
  }

  void renderFormattedTime(int v1, int v2, int v3, bool isTimeMode) {
    memset(buffer, 0, 32);

    drawDigit(0, v1 / 10);
    drawDigit(4, v1 % 10);

    if (isTimeMode) drawColon(8);
    else drawSlash(8);

    drawDigit(12, v2 / 10);
    drawDigit(16, v2 % 10);

    if (isTimeMode) drawColon(20);
    else drawSlash(20);

    drawDigit(24, v3 / 10);
    drawDigit(28, v3 % 10);

    show();
  }
};

Matrix8x32 display;

bool showTimeNotDate = true;
unsigned long lastButtonDebounce = 0;
bool lastButtonState = HIGH;

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 25200;
const int daylightOffset_sec = 0;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  display.init();
  display.drawWifi();

  WiFiManager wm;

  const char* customCSS =
      "<style>"
      "body{background-color:#121212;color:#e0e0e0;font-family:sans-serif;padding:20px;}"
      "h1{color:#00e676;font-size:24px;text-align:center;}"
      "input[type='text'],input[type='password']{width:100%;padding:12px;margin:8px 0;border-radius:6px;border:1px solid #333;background:#222;color:#fff;box-sizing:border-box;}"
      "button{background-color:#00e676;color:#000;font-weight:bold;padding:14px;border:none;border-radius:6px;width:100%;cursor:pointer;margin-top:10px;}"
      "</style>";

  wm.setCustomHeadElement(customCSS);
  wm.setConfigPortalTimeout(180);

  if (!wm.autoConnect("ESP32-Clock-Config")) {
    ESP.restart();
  }

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  bool currentButtonState = digitalRead(BUTTON_PIN);

  if (currentButtonState == LOW && lastButtonState == HIGH &&
      (millis() - lastButtonDebounce > 200)) {
    showTimeNotDate = !showTimeNotDate;
    lastButtonDebounce = millis();
  }

  lastButtonState = currentButtonState;

  struct tm timeinfo;

  if (getLocalTime(&timeinfo)) {
    if (showTimeNotDate) {
      display.renderFormattedTime(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, true);
    } else {
      display.renderFormattedTime(timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year % 100, false);
    }
  } else {
    display.drawWifi();
  }

  if (WiFi.status() != WL_CONNECTED) {
    display.drawWifi();
    WiFi.reconnect();
    delay(2000);
  }

  delay(50);
}
