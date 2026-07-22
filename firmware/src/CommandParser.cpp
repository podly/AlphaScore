/******************************************************************************
 * AlphaScore
 * ----------------------------------------------------------------------------
 * File:
 *     CommandParser.cpp
 *
 * Description:
 *     Parses myPinballs-compatible and extended AlphaScore serial commands.
 *
 * Copyright (c) 2026 Michal Podlejski
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include "CommandParser.h"

#include <string.h>

CommandParser::CommandParser(DisplayController &display)
  : display_(display) {
}

void CommandParser::parse(char *line) {
  if (line == nullptr || line[0] == '\0') {
    return;
  }

  if (line[0] == '@') {
    parseExtended(line);
  } else {
    parseLegacy(line);
  }
}

void CommandParser::parseLegacy(char *line) {
  char *firstColon = strchr(line, ':');
  if (firstColon == nullptr) {
    return;
  }

  *firstColon = '\0';
  char *secondColon = strchr(firstColon + 1, ':');
  if (secondColon == nullptr) {
    return;
  }

  *secondColon = '\0';
  char *modeStr = line;
  char *displayStr = firstColon + 1;
  char *textStr = secondColon + 1;

  if (modeStr == nullptr || displayStr == nullptr) {
    return;
  }

  const uint8_t mode = static_cast<uint8_t>(atoi(modeStr));
  const uint8_t display = static_cast<uint8_t>(atoi(displayStr));
  const bool autoDecimalPoints = mode == Config::ModeNormal || mode == Config::ModeFlash || mode == Config::ModeMatch;
  display_.setModeText(display, mode, textStr, autoDecimalPoints);
}

void CommandParser::parseExtended(char *line) {
  char *body = line + 1;
  char *firstColon = strchr(body, ':');
  if (firstColon == nullptr) {
    return;
  }

  *firstColon = '\0';
  char *secondColon = strchr(firstColon + 1, ':');

  char *commandStr = body;
  char *displayStr = firstColon + 1;
  char *payloadStr = nullptr;

  if (secondColon != nullptr) {
    *secondColon = '\0';
    payloadStr = secondColon + 1;
  }

  if (commandStr == nullptr || displayStr == nullptr) {
    return;
  }

  const char command = commandStr[0];
  const uint8_t display = static_cast<uint8_t>(atoi(displayStr));

  switch (command) {
    case 'R':
    case 'r':
      display_.renderRaw(display, payloadStr);
      break;

    case 'B':
    case 'b': {
      if (payloadStr == nullptr) {
        return;
      }
      const uint8_t brightness = static_cast<uint8_t>(atoi(payloadStr));
      if (display == 0) {
        display_.setAllBrightness(brightness);
      } else {
        display_.setBrightness(display, brightness);
      }
      break;
    }

    case 'C':
    case 'c':
      if (display == 0) {
        display_.clearAll();
      } else {
        display_.clearDisplay(display);
      }
      break;
  }
}
