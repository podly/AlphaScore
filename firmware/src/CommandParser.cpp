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

void CommandParser::setBrightnessCommandsEnabled(bool enabled) {
  brightnessCommandsEnabled_ = enabled;
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

bool CommandParser::parseUint8(const char *text, uint8_t minValue, uint8_t maxValue, uint8_t &value) const {
  if (text == nullptr || text[0] == '\0') {
    return false;
  }

  uint16_t parsed = 0;
  for (uint8_t i = 0; text[i] != '\0'; i++) {
    const char c = text[i];
    if (c < '0' || c > '9') {
      return false;
    }

    parsed = parsed * 10 + (c - '0');
    if (parsed > maxValue) {
      return false;
    }
  }

  if (parsed < minValue) {
    return false;
  }

  value = static_cast<uint8_t>(parsed);
  return true;
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

  uint8_t mode = 0;
  uint8_t display = 0;
  if (!parseUint8(modeStr, Config::ModeNormal, Config::ModeMatch, mode)
      || !parseUint8(displayStr, 1, Config::LogicalDisplays, display)) {
    return;
  }

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
  uint8_t display = 0;
  if (!parseUint8(displayStr, 0, Config::LogicalDisplays, display)) {
    return;
  }

  switch (command) {
    case 'R':
    case 'r':
      if (display == 0) {
        return;
      }
      display_.renderRaw(display, payloadStr);
      break;

    case 'B':
    case 'b': {
      if (!brightnessCommandsEnabled_ || payloadStr == nullptr) {
        return;
      }
      uint8_t brightness = 0;
      if (!parseUint8(payloadStr, Config::BrightnessMin, Config::BrightnessMax, brightness)) {
        return;
      }
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
