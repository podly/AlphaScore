#pragma once

#include <Arduino.h>

class Jumpers {
public:
  void begin();
  uint8_t readMask() const;
};
