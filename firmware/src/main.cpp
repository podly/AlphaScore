/******************************************************************************
 * AlphaScore
 * ----------------------------------------------------------------------------
 * File:
 *     main.cpp
 *
 * Description:
 *     Provides the Arduino setup and loop entry points for AlphaScore.
 *
 * Copyright (c) 2026 Michal Podlejski
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include <Arduino.h>
#include "App.h"

void setup() {
  appSetup();
}

void loop() {
  appLoop();
}
