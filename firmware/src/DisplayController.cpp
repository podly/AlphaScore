/******************************************************************************
 * AlphaScore
 * ----------------------------------------------------------------------------
 * File:
 *     DisplayController.cpp
 *
 * Description:
 *     Implements text, raw-segment, brightness and blink control for logical displays.
 *
 * Copyright (c) 2026 Michal Podlejski
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include "DisplayController.h"

void DisplayController::begin() {
  driver_.begin();

  for (uint8_t i = 0; i < Config::LogicalDisplays; i++) {
    displays_[i].mode = Config::ModeClear;
    displays_[i].autoDecimalPoints = false;
    clearText(displays_[i].text);
  }
}

void DisplayController::setFontStyle(SegmentFont::Style style) {
  fontStyle_ = style;
  refreshAll();
}

void DisplayController::setModeText(uint8_t displayOneBased, uint8_t mode, const char *text, bool autoDecimalPoints) {
  if (displayOneBased < 1 || displayOneBased > Config::LogicalDisplays) {
    return;
  }
  if (mode < Config::ModeNormal || mode > Config::ModeMatch) {
    return;
  }

  const uint8_t display = displayOneBased - 1;
  displays_[display].mode = mode;
  displays_[display].autoDecimalPoints = autoDecimalPoints;

  if (mode == Config::ModeClear) {
    displays_[display].autoDecimalPoints = false;
    clearText(displays_[display].text);
    driver_.clearDisplay(display);
    return;
  }

  copyText(displays_[display].text, text ? text : "");
  refresh(display);
}

void DisplayController::renderRaw(uint8_t displayOneBased, char *payload) {
  if (displayOneBased < 1 || displayOneBased > Config::LogicalDisplays || payload == nullptr) {
    return;
  }

  SegmentFont::Mask masks[Config::CharactersPerDisplay] = {};
  char *token = strtok(payload, ",");
  for (uint8_t i = 0; i < Config::CharactersPerDisplay; i++) {
    uint16_t raw = 0;
    bool wideToken = false;
    if (token) {
      wideToken = isWideHexToken(token);
      if (!parseHexWord(token, raw)) {
        return;
      }
      token = strtok(nullptr, ",");
    }

    masks[i] = !wideToken
      ? SegmentFont::fromSevenSegment(static_cast<uint8_t>(raw))
      : raw;
  }

  const uint8_t display = displayOneBased - 1;
  displays_[display].mode = Config::ModeRaw;
  displays_[display].autoDecimalPoints = false;
  clearText(displays_[display].text);

  for (uint8_t i = 0; i < Config::CharactersPerDisplay; i++) {
    const uint8_t physicalPosition = Config::ReverseDigitOrder
      ? i
      : Config::CharactersPerDisplay - 1 - i;
    driver_.setCharacter(display, physicalPosition, masks[i]);
  }
}

void DisplayController::clearDisplay(uint8_t displayOneBased) {
  if (displayOneBased < 1 || displayOneBased > Config::LogicalDisplays) {
    return;
  }

  const uint8_t display = displayOneBased - 1;
  displays_[display].mode = Config::ModeClear;
  displays_[display].autoDecimalPoints = false;
  clearText(displays_[display].text);
  driver_.clearDisplay(display);
}

void DisplayController::clearAll() {
  for (uint8_t display = 1; display <= Config::LogicalDisplays; display++) {
    clearDisplay(display);
  }
}

void DisplayController::setBrightness(uint8_t displayOneBased, uint8_t brightness) {
  if (displayOneBased < 1 || displayOneBased > Config::LogicalDisplays) {
    return;
  }

  const uint8_t firstDriver = (displayOneBased - 1) * Config::DriversPerDisplay;
  for (uint8_t i = 0; i < Config::DriversPerDisplay; i++) {
    driver_.setBrightness(firstDriver + i, brightness);
  }
}

void DisplayController::setAllBrightness(uint8_t brightness) {
  driver_.setAllBrightness(brightness);
}

void DisplayController::updateBlink() {
  const unsigned long interval = blinkOn_ ? Config::BlinkOnMs : Config::BlinkOffMs;
  if (millis() - lastBlinkTime_ < interval) {
    return;
  }

  lastBlinkTime_ = millis();
  blinkOn_ = !blinkOn_;
  refreshAll();
}

void DisplayController::refreshAll() {
  for (uint8_t i = 0; i < Config::LogicalDisplays; i++) {
    refresh(i);
  }
}


void DisplayController::showDemoCharacters(uint8_t startPosition, const char *text) {
  char desired[Config::CharactersPerDisplay + 1];
  clearText(desired);

  for (uint8_t i = 0; i < Config::CharactersPerDisplay; i++) {
    const uint8_t position = startPosition + i;
    if (position >= Config::CharactersPerDisplay) {
      break;
    }

    desired[position] = text && text[i] != '\0' ? text[i] : ' ';
  }

  const bool forceRefresh = displays_[0].mode != Config::ModeNormal;
  displays_[0].mode = Config::ModeNormal;
  displays_[0].autoDecimalPoints = false;

  for (uint8_t position = 0; position < Config::CharactersPerDisplay; position++) {
    const char c = desired[position];
    if (!forceRefresh && displays_[0].text[position] == c) {
      continue;
    }

    displays_[0].text[position] = c;
    driver_.setCharacter(0, position, SegmentFont::encode(c, false, fontStyle_));
  }
}
void DisplayController::selfTest() {
  constexpr const char *startupText[Config::LogicalDisplays] = {
    "        ",
    " * ALPHA",
    "SCORE * ",
    "        "
  };

  for (uint8_t display = 0; display < Config::LogicalDisplays; display++) {
    for (uint8_t pos = 0; pos < Config::CharactersPerDisplay; pos++) {
      driver_.setCharacter(display, pos, SegmentFont::encode(startupText[display][pos], false, fontStyle_));
    }
  }

  delay(Config::SelfTestMessageMs);
  clearAll();
}

void DisplayController::clearText(char *text) {
  for (uint8_t i = 0; i < Config::CharactersPerDisplay; i++) {
    text[i] = ' ';
  }
  text[Config::CharactersPerDisplay] = '\0';
}

void DisplayController::copyText(char *dest, const char *src) {
  clearText(dest);

  for (uint8_t i = 0; i < Config::CharactersPerDisplay && src[i] != '\0'; i++) {
    dest[i] = src[i];
  }
}

void DisplayController::refresh(uint8_t display) {
  if (display >= Config::LogicalDisplays) {
    return;
  }

  const DisplayState &state = displays_[display];

  if (state.mode == Config::ModeRaw) {
    return;
  }
  if (state.mode == Config::ModeClear) {
    driver_.clearDisplay(display);
    return;
  }
  if (state.mode == Config::ModeFlash && !blinkOn_) {
    driver_.clearDisplay(display);
    return;
  }

  drawText(display, state.text, state.mode == Config::ModeMatch, state.autoDecimalPoints);
}

void DisplayController::drawText(uint8_t display, const char *text, bool matchMode, bool autoDecimalPoints) {
  const bool numericOnly = autoDecimalPoints && isNumericText(text);

  for (uint8_t i = 0; i < Config::CharactersPerDisplay; i++) {
    const uint8_t physicalPosition = Config::ReverseDigitOrder
      ? i
      : Config::CharactersPerDisplay - 1 - i;

    const char c = text[i];
    bool decimalPoint = false;

    if (numericOnly && c >= '0' && c <= '9') {
      uint8_t digitsToRight = 0;
      for (uint8_t j = i + 1; j < Config::CharactersPerDisplay; j++) {
        if (text[j] >= '0' && text[j] <= '9') {
          digitsToRight++;
        }
      }
      decimalPoint = digitsToRight > 0 && digitsToRight % 3 == 0;
    }

    const bool hiddenByMatch = matchMode && i >= Config::CharactersPerDisplay - 2 && !blinkOn_;
    const SegmentFont::Mask mask = hiddenByMatch
      ? 0
      : SegmentFont::encode(c, decimalPoint, fontStyle_);
    driver_.setCharacter(display, physicalPosition, mask);
  }
}

bool DisplayController::isNumericText(const char *text) const {
  bool hasDigit = false;

  for (uint8_t i = 0; i < Config::CharactersPerDisplay; i++) {
    const char c = text[i];
    if (c == ' ' || c == '?') {
      continue;
    }
    if (c < '0' || c > '9') {
      return false;
    }
    hasDigit = true;
  }

  return hasDigit;
}

bool DisplayController::parseHexWord(const char *token, uint16_t &value) const {
  if (token == nullptr || token[0] == '\0') {
    return false;
  }

  value = 0;

  uint8_t length = 0;
  for (; length < 4 && token[length] != '\0'; length++) {
    const char c = token[length];
    uint8_t digit = 0;

    if (c >= '0' && c <= '9') {
      digit = c - '0';
    } else if (c >= 'A' && c <= 'F') {
      digit = c - 'A' + 10;
    } else if (c >= 'a' && c <= 'f') {
      digit = c - 'a' + 10;
    } else {
      return false;
    }

    value = (value << 4) | digit;
  }

  return token[length] == '\0';
}

bool DisplayController::isWideHexToken(const char *token) const {
  uint8_t digits = 0;

  while (token[digits] != '\0') {
    digits++;
  }

  return digits > 2;
}



