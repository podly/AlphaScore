/******************************************************************************
 * AlphaScore
 * ----------------------------------------------------------------------------
 * File:
 *     SerialLineReader.cpp
 *
 * Description:
 *     Implements buffered, line-oriented command input from the UART interface.
 *
 * Copyright (c) 2026 Michal Podlejski
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include "SerialLineReader.h"

bool SerialLineReader::readLine(char *out, uint8_t outSize) {
  while (Serial.available()) {
    const char c = static_cast<char>(Serial.read());

    if (c == '\r') {
      continue;
    }

    if (c == '\n') {
      if (discardingLine_) {
        discardingLine_ = false;
        pos_ = 0;
        continue;
      }

      buffer_[pos_] = '\0';

      if (outSize > 0) {
        uint8_t i = 0;
        for (; i < outSize - 1 && buffer_[i] != '\0'; i++) {
          out[i] = buffer_[i];
        }
        out[i] = '\0';
      }

      pos_ = 0;
      return true;
    }

    if (discardingLine_) {
      continue;
    }

    if (pos_ < BufferSize - 1) {
      buffer_[pos_++] = c;
    } else {
      discardingLine_ = true;
      pos_ = 0;
    }
  }

  return false;
}
