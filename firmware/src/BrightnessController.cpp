/******************************************************************************
 * AlphaScore
 * ----------------------------------------------------------------------------
 * File:
 *     BrightnessController.cpp
 *
 * Description:
 *     Reads the brightness control and applies its value to the displays.
 *
 * Copyright (c) 2026 Michal Podlejski
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include "BrightnessController.h"

#include "Config.h"

BrightnessController::BrightnessController(DisplayController &display)
  : display_(display) {
}

void BrightnessController::begin() {
  pinMode(Config::BrightnessPotPin, INPUT);
  update(true);
}

void BrightnessController::update(bool force) {
  if (!force && millis() - lastUpdate_ < Config::BrightnessUpdateMs) {
    return;
  }

  lastUpdate_ = millis();

  const int raw = analogRead(Config::BrightnessPotPin);
  uint8_t brightness = static_cast<uint8_t>(map(
    raw,
    0,
    1023,
    Config::BrightnessMin,
    Config::BrightnessMax
  ));

  if (brightness > 15) {
    brightness = 15;
  }

  if (!force && brightness == currentBrightness_) {
    return;
  }

  currentBrightness_ = brightness;
  display_.setAllBrightness(currentBrightness_);
}
