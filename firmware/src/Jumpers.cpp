/******************************************************************************
 * AlphaScore
 * ----------------------------------------------------------------------------
 * File:
 *     Jumpers.cpp
 *
 * Description:
 *     Reads the onboard configuration jumpers and builds their state mask.
 *
 * Copyright (c) 2026 Michal Podlejski
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include "Jumpers.h"

#include "Config.h"

void Jumpers::begin() {
  pinMode(Config::Jp24Pin, INPUT_PULLUP);
  pinMode(Config::Jp25Pin, INPUT_PULLUP);
  pinMode(Config::Jp26Pin, INPUT_PULLUP);
  pinMode(Config::Jp27Pin, INPUT_PULLUP);
}

uint8_t Jumpers::readMask() const {
  uint8_t mask = 0;

  if (digitalRead(Config::Jp24Pin) == LOW) mask |= 0x01;
  if (digitalRead(Config::Jp25Pin) == LOW) mask |= 0x02;
  if (digitalRead(Config::Jp26Pin) == LOW) mask |= 0x04;
  if (digitalRead(Config::Jp27Pin) == LOW) mask |= 0x08;

  return mask;
}
