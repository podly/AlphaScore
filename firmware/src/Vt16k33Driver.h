#pragma once

#include <Arduino.h>
#include "Config.h"
#include "SegmentFont.h"

class Vt16k33Driver {
public:
  void begin();
  void clearAll();
  void setBrightness(uint8_t driver, uint8_t brightness);
  void setAllBrightness(uint8_t brightness);
  void setCharacter(uint8_t logicalDisplay, uint8_t position, SegmentFont::Mask mask);
  void clearDisplay(uint8_t logicalDisplay);
  void flushAll();

private:
  uint16_t ram_[Config::DriverCount][Config::Vt16k33RamRows] = {};

  uint8_t addressForDriver(uint8_t driver) const;
  uint8_t driverFor(uint8_t logicalDisplay, uint8_t position) const;
  uint8_t ramRowFor(uint8_t position) const;
  void writeCommand(uint8_t driver, uint8_t command);
  void flushDriver(uint8_t driver);
};
