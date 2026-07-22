/******************************************************************************
 * AlphaScore
 * ----------------------------------------------------------------------------
 * File:
 *     App.cpp
 *
 * Description:
 *     Coordinates application startup, serial commands, display updates and Demo Mode.
 *
 * Copyright (c) 2026 Michal Podlejski
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include "App.h"

#include <Arduino.h>
#include "BrightnessController.h"
#include "CommandParser.h"
#include "Config.h"
#include "DemoMode.h"
#include "DisplayController.h"
#include "Jumpers.h"
#include "SerialLineReader.h"

namespace {

DisplayController display;
BrightnessController brightness(display);
CommandParser parser(display);
SerialLineReader serialReader;
Jumpers jumpers;
DemoMode demo(display);

char line[96];
uint8_t jumperMask = 0;
bool demoMode = false;
bool fancyFont = false;
bool hardwareBrightness = false;

bool parseDemoCommand(char *line, bool &enabled) {
  if (line == nullptr || line[0] != '@') {
    return false;
  }
  if (line[1] != 'X' && line[1] != 'x') {
    return false;
  }
  if (line[2] != ':') {
    return false;
  }

  if (line[3] == '1' && line[4] == '\0') {
    enabled = true;
    return true;
  }
  if (line[3] == '0' && line[4] == '\0') {
    enabled = false;
    return true;
  }

  return false;
}

void setDemoMode(bool enabled) {
  if (demoMode == enabled) {
    return;
  }

  demoMode = enabled;
  if (demoMode) {
    demo.begin();
  } else {
    display.setFontStyle(fancyFont ? SegmentFont::Style::Fancy : SegmentFont::Style::Standard);
    display.clearAll();
  }
}

}

void appSetup() {
  Serial.begin(Config::SerialBaud);

  jumpers.begin();
  jumperMask = jumpers.readMask();
  hardwareBrightness = (jumperMask & 0x01) != 0;
  demoMode = (jumperMask & 0x08) != 0;
  fancyFont = (jumperMask & 0x04) != 0;

  display.begin();
  display.setFontStyle(fancyFont ? SegmentFont::Style::Fancy : SegmentFont::Style::Standard);
  parser.setBrightnessCommandsEnabled(!hardwareBrightness);
  if (hardwareBrightness) {
    brightness.begin();
  }

  if (demoMode) {
    demo.begin();
    return;
  }

  if (Config::SelfTestEnabled) {
    display.selfTest();
  }
}

void appLoop() {
  if (hardwareBrightness) {
    brightness.update();
  }

  const bool hasLine = serialReader.readLine(line, sizeof(line));
  if (hasLine) {
    bool requestedDemoMode = false;
    if (parseDemoCommand(line, requestedDemoMode)) {
      setDemoMode(requestedDemoMode);
    } else if (!demoMode) {
      parser.parse(line);
    }
  }

  if (demoMode) {
    demo.update();
    return;
  }

  display.updateBlink();
}
