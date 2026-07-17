#pragma once

#include <Arduino.h>
#include "DisplayController.h"

class BrightnessController {
public:
  explicit BrightnessController(DisplayController &display);
  void begin();
  void update(bool force = false);

private:
  DisplayController &display_;
  unsigned long lastUpdate_ = 0;
  uint8_t currentBrightness_ = 255;
};
