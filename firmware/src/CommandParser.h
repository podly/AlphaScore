/******************************************************************************
 * AlphaScore
 * ----------------------------------------------------------------------------
 * File:
 *     CommandParser.h
 *
 * Description:
 *     Declares the AlphaScore serial command parser.
 *
 * Copyright (c) 2026 Michal Podlejski
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#pragma once

#include <Arduino.h>
#include "DisplayController.h"

class CommandParser {
public:
  explicit CommandParser(DisplayController &display);
  void setBrightnessCommandsEnabled(bool enabled);
  void parse(char *line);

private:
  DisplayController &display_;
  bool brightnessCommandsEnabled_ = true;

  bool parseUint8(const char *text, uint8_t minValue, uint8_t maxValue, uint8_t &value) const;
  void parseLegacy(char *line);
  void parseExtended(char *line);
};
