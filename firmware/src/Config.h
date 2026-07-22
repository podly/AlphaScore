/******************************************************************************
 * AlphaScore
 * ----------------------------------------------------------------------------
 * File:
 *     Config.h
 *
 * Description:
 *     Defines compile-time hardware, display and timing configuration constants.
 *
 * Copyright (c) 2026 Michal Podlejski
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#pragma once

#include <Arduino.h>

namespace Config {

constexpr uint32_t SerialBaud = 115200;

constexpr uint8_t LogicalDisplays = 4;
constexpr uint8_t DriversPerDisplay = 2;
constexpr uint8_t CharactersPerDriver = 4;
constexpr uint8_t CharactersPerDisplay = DriversPerDisplay * CharactersPerDriver;
constexpr uint8_t DriverCount = LogicalDisplays * DriversPerDisplay;
constexpr uint8_t Vt16k33RamRows = 8;
constexpr uint8_t CharacterRamRow[CharactersPerDriver] = {0, 1, 2, 3};

constexpr uint8_t Vt16k33BaseAddress = 0x70;

constexpr uint8_t BrightnessPotPin = A0;
constexpr uint8_t BrightnessMin = 0;
constexpr uint8_t BrightnessMax = 15;
constexpr uint8_t DefaultBrightness = 4;
constexpr uint16_t BrightnessUpdateMs = 100;

constexpr uint8_t Jp24Pin = 2; // BRIGHTNESS
constexpr uint8_t Jp25Pin = 3; // RESERVED FOR FUTURE USE
constexpr uint8_t Jp26Pin = 4; // MYPINBALLS FANCY FONT
constexpr uint8_t Jp27Pin = 5; // DEMO

constexpr uint16_t BlinkOnMs = 500;
constexpr uint16_t BlinkOffMs = 200;

constexpr bool SelfTestEnabled = true;
constexpr uint16_t SelfTestMessageMs = 3000;

constexpr bool ReverseDigitOrder = true;

constexpr uint8_t ModeNormal = 1;
constexpr uint8_t ModeFlash = 2;
constexpr uint8_t ModeClear = 3;
constexpr uint8_t ModeMatch = 4;
constexpr uint8_t ModeRaw = 5;

}
