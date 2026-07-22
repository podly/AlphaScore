/******************************************************************************
 * AlphaScore
 * ----------------------------------------------------------------------------
 * File:
 *     Jumpers.h
 *
 * Description:
 *     Declares access to the onboard configuration jumpers.
 *
 * Copyright (c) 2026 Michal Podlejski
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#pragma once

#include <Arduino.h>

class Jumpers {
public:
  void begin();
  uint8_t readMask() const;
};
