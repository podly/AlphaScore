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
  void parse(char *line);

private:
  DisplayController &display_;

  void parseLegacy(char *line);
  void parseExtended(char *line);
};
