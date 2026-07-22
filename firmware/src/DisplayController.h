/******************************************************************************
 * AlphaScore
 * ----------------------------------------------------------------------------
 * File:
 *     DisplayController.h
 *
 * Description:
 *     Declares high-level control of the AlphaScore logical displays.
 *
 * Copyright (c) 2026 Michal Podlejski
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#pragma once

#include <Arduino.h>
#include "Config.h"
#include "SegmentFont.h"
#include "Vt16k33Driver.h"

class DisplayController {
public:
  void begin();
  void setFontStyle(SegmentFont::Style style);
  void setModeText(uint8_t displayOneBased, uint8_t mode, const char *text, bool autoDecimalPoints = false);
  void renderRaw(uint8_t displayOneBased, char *payload);
  void clearDisplay(uint8_t displayOneBased);
  void clearAll();
  void setBrightness(uint8_t displayOneBased, uint8_t brightness);
  void setAllBrightness(uint8_t brightness);
  void updateBlink();
  void refreshAll();
  void selfTest();
  void showDemoCharacters(uint8_t startPosition, const char *text);

private:
  struct DisplayState {
    uint8_t mode = Config::ModeClear;
    char text[Config::CharactersPerDisplay + 1] = {};
    bool autoDecimalPoints = false;
  };

  Vt16k33Driver driver_;
  DisplayState displays_[Config::LogicalDisplays];
  SegmentFont::Style fontStyle_ = SegmentFont::Style::Standard;
  unsigned long lastBlinkTime_ = 0;
  bool blinkOn_ = true;

  void clearText(char *text);
  void copyText(char *dest, const char *src);
  void refresh(uint8_t display);
  void drawText(uint8_t display, const char *text, bool matchMode, bool autoDecimalPoints);
  bool isNumericText(const char *text) const;
  bool parseHexWord(const char *token, uint16_t &value) const;
  bool isWideHexToken(const char *token) const;
};

