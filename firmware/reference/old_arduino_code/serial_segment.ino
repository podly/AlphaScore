/******************************************************************************
 * AlphaScore
 * ----------------------------------------------------------------------------
 * File:
 *     serial_segment.ino
 *
 * Description:
 *     Preserves the original serial display controller sketch for reference.
 *
 * Copyright (c) 2026 Michal Podlejski
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include <LedControl.h>

#define DIN_PIN 11
#define CLK_PIN 13
#define CS_PIN  10

#define NUM_MODULES 4
#define DIGITS_PER_MODULE 8

#define SERIAL_BAUD 115200

#define DEFAULT_INTENSITY 1

#define USE_BRIGHTNESS_POT false
#define BRIGHTNESS_POT_PIN A0
#define BRIGHTNESS_MIN 1
#define BRIGHTNESS_MAX 10
#define BRIGHTNESS_UPDATE_MS 100

#define BLINK_ON_MS  500
#define BLINK_OFF_MS 200

#define SELFTEST_ENABLED true
#define SELFTEST_SEGMENTS_MS 1000
#define SELFTEST_ID_MS 2000

#define REVERSE_DIGIT_ORDER false

#define MODE_RAW 5

LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, NUM_MODULES);

struct DisplayState {
  uint8_t mode;      // 1 - normal, 2 - flash, 3 - clear, 4 - match
  char text[9];      // 8 chars + null
};

DisplayState displays[NUM_MODULES];

char serialBuffer[80];
uint8_t serialPos = 0;

unsigned long lastBlinkTime = 0;
bool blinkState = true;

unsigned long lastBrightnessUpdate = 0;
uint8_t currentIntensity = 255;

void setup() {
  Serial.begin(SERIAL_BAUD);

  for (uint8_t i = 0; i < NUM_MODULES; i++) {
    lc.shutdown(i, false);
    lc.setIntensity(i, DEFAULT_INTENSITY);
    lc.clearDisplay(i);

    displays[i].mode = 3;
    clearText(displays[i].text);
  }

#if SELFTEST_ENABLED
  runSelfTest();
#endif

  applyInitialBrightness();
}

void loop() {
  readSerial();
  updateBrightness();
  updateBlink();
}

void readSerial() {
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\r') {
      continue;
    }

    if (c == '\n') {
      serialBuffer[serialPos] = '\0';
      parseCommand(serialBuffer);
      serialPos = 0;
      continue;
    }

    if (serialPos < sizeof(serialBuffer) - 1) {
      serialBuffer[serialPos++] = c;
    } else {
      serialPos = 0;
    }
  }
}

void parseCommand(char *cmd) {
  if (cmd[0] == '@') {
    parseSerialSegment(cmd);
    return;
  }

  parseMyPinballs(cmd);
}

void parseMyPinballs(char *cmd) {
  char *modeStr = strtok(cmd, ":");
  char *displayStr = strtok(NULL, ":");
  char *textStr = strtok(NULL, "");

  if (!modeStr || !displayStr) {
    return;
  }

  uint8_t mode = atoi(modeStr);
  uint8_t displayNum = atoi(displayStr);

  if (mode < 1 || mode > 4) {
    return;
  }

  if (displayNum < 1 || displayNum > NUM_MODULES) {
    return;
  }

  uint8_t idx = displayNum - 1;
  displays[idx].mode = mode;

  if (mode == 3) {
    clearText(displays[idx].text);
    displays[idx].mode = 3;
    lc.clearDisplay(idx);
    return;
  }

  if (textStr) {
    setDisplayText(displays[idx].text, textStr);
  } else {
    clearText(displays[idx].text);
  }

  refreshDisplay(idx);
}

void parseSerialSegment(char *cmd) {
  char *commandStr = strtok(cmd + 1, ":");
  char *displayStr = strtok(NULL, ":");
  char *payloadStr = strtok(NULL, "");

  if (!commandStr || !displayStr) {
    return;
  }

  char command = commandStr[0];
  uint8_t displayNum = atoi(displayStr);

  if (command == 'R') {
    if (!payloadStr) {
      return;
    }

    if (displayNum < 1 || displayNum > NUM_MODULES) {
      return;
    }

    renderRawDisplay(displayNum - 1, payloadStr);
    return;
  }

  if (command == 'B') {
    if (!payloadStr) {
      return;
    }

    uint8_t intensity = atoi(payloadStr);

    if (displayNum == 0) {
      setAllIntensity(intensity);
      currentIntensity = intensity;
      return;
    }

    if (displayNum < 1 || displayNum > NUM_MODULES) {
      return;
    }

    setDisplayIntensity(displayNum - 1, intensity);
    return;
  }

  if (command == 'C') {
    if (displayNum == 0) {
      clearAllDisplays();
      return;
    }

    if (displayNum < 1 || displayNum > NUM_MODULES) {
      return;
    }

    clearRawDisplay(displayNum - 1);
    return;
  }
}

void renderRawDisplay(uint8_t module, char *payload) {
  displays[module].mode = MODE_RAW;
  clearText(displays[module].text);

  char *token = strtok(payload, ",");

  for (uint8_t i = 0; i < DIGITS_PER_MODULE; i++) {
    uint8_t mask = 0x00;

    if (token) {
      mask = parseHexByte(token);
      token = strtok(NULL, ",");
    }

    uint8_t physicalPos = REVERSE_DIGIT_ORDER ? i : 7 - i;
    lc.setRow(module, physicalPos, mapRawMaskToLedControl(mask));
  }
}

uint8_t mapRawMaskToLedControl(uint8_t mask) {
  uint8_t out = 0;

  if (mask & 0x01) out |= 0x40; // A
  if (mask & 0x02) out |= 0x20; // B
  if (mask & 0x04) out |= 0x10; // C
  if (mask & 0x08) out |= 0x08; // D
  if (mask & 0x10) out |= 0x04; // E
  if (mask & 0x20) out |= 0x02; // F
  if (mask & 0x40) out |= 0x01; // G
  if (mask & 0x80) out |= 0x80; // DP

  return out;
}

uint8_t parseHexByte(const char *s) {
  uint8_t value = 0;

  for (uint8_t i = 0; i < 2 && s[i] != '\0'; i++) {
    char c = s[i];

    value <<= 4;

    if (c >= '0' && c <= '9') {
      value |= c - '0';
    } else if (c >= 'A' && c <= 'F') {
      value |= c - 'A' + 10;
    } else if (c >= 'a' && c <= 'f') {
      value |= c - 'a' + 10;
    }
  }

  return value;
}

void clearRawDisplay(uint8_t module) {
  displays[module].mode = 3;
  clearText(displays[module].text);
  lc.clearDisplay(module);
}

void clearAllDisplays() {
  for (uint8_t i = 0; i < NUM_MODULES; i++) {
    clearRawDisplay(i);
  }
}

void setDisplayIntensity(uint8_t module, uint8_t intensity) {
  if (intensity > 15) {
    intensity = 15;
  }

  lc.setIntensity(module, intensity);
}

void setDisplayText(char *dest, const char *src) {
  clearText(dest);

  for (uint8_t i = 0; i < 8 && src[i] != '\0'; i++) {
    char c = src[i];

    if (c == ' ') {
      c = '?';
    }

    dest[i] = c;
  }

  dest[8] = '\0';
}

void clearText(char *text) {
  for (uint8_t i = 0; i < 8; i++) {
    text[i] = '?';
  }

  text[8] = '\0';
}

void updateBlink() {
  unsigned long interval = blinkState ? BLINK_ON_MS : BLINK_OFF_MS;

  if (millis() - lastBlinkTime >= interval) {
    lastBlinkTime = millis();
    blinkState = !blinkState;
    refreshAllDisplays();
  }
}

void refreshAllDisplays() {
  for (uint8_t i = 0; i < NUM_MODULES; i++) {
    refreshDisplay(i);
  }
}

void refreshDisplay(uint8_t module) {
  DisplayState &d = displays[module];

  if (d.mode == MODE_RAW) {
    return;
  }

  if (d.mode == 3) {
    lc.clearDisplay(module);
    return;
  }

  if (d.mode == 2 && !blinkState) {
    lc.clearDisplay(module);
    return;
  }

  drawText(module, d.text, d.mode == 4);
}

void drawText(uint8_t module, const char *text, bool matchMode) {
//  lc.clearDisplay(module);

  bool numericOnly = isNumericText(text);

  for (uint8_t i = 0; i < 8; i++) {
    uint8_t physicalPos = REVERSE_DIGIT_ORDER ? i : 7 - i;
    char c = text[i];

    bool dp = false;

    if (numericOnly && c >= '0' && c <= '9') {
      uint8_t digitsToRight = 0;

      for (uint8_t j = i + 1; j < 8; j++) {
        if (text[j] >= '0' && text[j] <= '9') {
          digitsToRight++;
        }
      }

      if (digitsToRight > 0 && digitsToRight % 3 == 0) {
        dp = true;
      }
    }

    if (matchMode && i >= 6 && !blinkState) {
      lc.setChar(module, physicalPos, ' ', false);
      continue;
    }

    if (c == '?' || c == ' ') {
      lc.setChar(module, physicalPos, ' ', false);
    } else if (c >= '0' && c <= '9') {
      lc.setDigit(module, physicalPos, c - '0', dp);
    } else if (c == '-') {
      lc.setChar(module, physicalPos, '-', dp);
    } else {
      lc.setChar(module, physicalPos, ' ', false);
    }
  }
}

bool isNumericText(const char *text) {
  bool hasDigit = false;

  for (uint8_t i = 0; i < 8; i++) {
    if (text[i] == '?' || text[i] == ' ') {
      continue;
    }

    if (text[i] < '0' || text[i] > '9') {
      return false;
    }

    hasDigit = true;
  }

  return hasDigit;
}

void applyInitialBrightness() {
#if USE_BRIGHTNESS_POT
  updateBrightness(true);
#else
  setAllIntensity(DEFAULT_INTENSITY);
#endif
}

void updateBrightness() {
  updateBrightness(false);
}

void updateBrightness(bool force) {
#if USE_BRIGHTNESS_POT
  if (!force && millis() - lastBrightnessUpdate < BRIGHTNESS_UPDATE_MS) {
    return;
  }

  lastBrightnessUpdate = millis();

  int raw = analogRead(BRIGHTNESS_POT_PIN);

  uint8_t newIntensity = map(
    raw,
    0,
    1023,
    BRIGHTNESS_MIN,
    BRIGHTNESS_MAX
  );

  if (!force && newIntensity == currentIntensity) {
    return;
  }

  currentIntensity = newIntensity;
  setAllIntensity(currentIntensity);
#else
  if (force) {
    currentIntensity = DEFAULT_INTENSITY;
    setAllIntensity(currentIntensity);
  }
#endif
}

void setAllIntensity(uint8_t intensity) {
  if (intensity > 15) {
    intensity = 15;
  }

  for (uint8_t i = 0; i < NUM_MODULES; i++) {
    lc.setIntensity(i, intensity);
  }
}

void runSelfTest() {
  for (uint8_t m = 0; m < NUM_MODULES; m++) {
    for (uint8_t d = 0; d < DIGITS_PER_MODULE; d++) {
      lc.setDigit(m, d, 8, true);
    }
  }

  delay(SELFTEST_SEGMENTS_MS);

  for (uint8_t m = 0; m < NUM_MODULES; m++) {
    for (uint8_t d = 0; d < DIGITS_PER_MODULE; d++) {
      lc.setDigit(m, d, m + 1, false);
    }
  }

  delay(SELFTEST_ID_MS);

  for (uint8_t m = 0; m < NUM_MODULES; m++) {
    lc.clearDisplay(m);
  }
}
