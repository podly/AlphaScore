#pragma once

#include <Arduino.h>

class SerialLineReader {
public:
  bool readLine(char *out, uint8_t outSize);

private:
  static constexpr uint8_t BufferSize = 96;
  char buffer_[BufferSize] = {};
  uint8_t pos_ = 0;
};
