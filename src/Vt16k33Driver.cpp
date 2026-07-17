#include "Vt16k33Driver.h"

#include <Wire.h>

void Vt16k33Driver::begin() {
  Wire.begin();

  for (uint8_t driver = 0; driver < Config::DriverCount; driver++) {
    writeCommand(driver, 0x21); // oscillator on
    writeCommand(driver, 0x81); // display on, blink off
    setBrightness(driver, Config::DefaultBrightness);
  }

  clearAll();
}

void Vt16k33Driver::clearAll() {
  for (uint8_t display = 0; display < Config::LogicalDisplays; display++) {
    clearDisplay(display);
  }
  flushAll();
}

void Vt16k33Driver::setBrightness(uint8_t driver, uint8_t brightness) {
  if (driver >= Config::DriverCount) {
    return;
  }

  if (brightness > 15) {
    brightness = 15;
  }

  writeCommand(driver, 0xE0 | brightness);
}

void Vt16k33Driver::setAllBrightness(uint8_t brightness) {
  for (uint8_t driver = 0; driver < Config::DriverCount; driver++) {
    setBrightness(driver, brightness);
  }
}

void Vt16k33Driver::setCharacter(uint8_t logicalDisplay, uint8_t position, SegmentFont::Mask mask) {
  if (logicalDisplay >= Config::LogicalDisplays || position >= Config::CharactersPerDisplay) {
    return;
  }

  const uint8_t driver = driverFor(logicalDisplay, position);
  const uint8_t row = ramRowFor(position);
  ram_[driver][row] = mask;
  flushDriver(driver);
}

void Vt16k33Driver::clearDisplay(uint8_t logicalDisplay) {
  if (logicalDisplay >= Config::LogicalDisplays) {
    return;
  }

  const uint8_t firstDriver = logicalDisplay * Config::DriversPerDisplay;
  for (uint8_t offset = 0; offset < Config::DriversPerDisplay; offset++) {
    const uint8_t driver = firstDriver + offset;
    for (uint8_t row = 0; row < Config::Vt16k33RamRows; row++) {
      ram_[driver][row] = 0;
    }
    flushDriver(driver);
  }
}

void Vt16k33Driver::flushAll() {
  for (uint8_t driver = 0; driver < Config::DriverCount; driver++) {
    flushDriver(driver);
  }
}

uint8_t Vt16k33Driver::addressForDriver(uint8_t driver) const {
  return Config::Vt16k33BaseAddress + driver;
}

uint8_t Vt16k33Driver::driverFor(uint8_t logicalDisplay, uint8_t position) const {
  return logicalDisplay * Config::DriversPerDisplay + position / Config::CharactersPerDriver;
}

uint8_t Vt16k33Driver::ramRowFor(uint8_t position) const {
  const uint8_t character = position % Config::CharactersPerDriver;
  return Config::CharacterRamRow[character];
}

void Vt16k33Driver::writeCommand(uint8_t driver, uint8_t command) {
  Wire.beginTransmission(addressForDriver(driver));
  Wire.write(command);
  Wire.endTransmission();
}

void Vt16k33Driver::flushDriver(uint8_t driver) {
  if (driver >= Config::DriverCount) {
    return;
  }

  Wire.beginTransmission(addressForDriver(driver));
  Wire.write(static_cast<uint8_t>(0x00));

  for (uint8_t row = 0; row < Config::Vt16k33RamRows; row++) {
    const uint16_t value = ram_[driver][row];
    Wire.write(static_cast<uint8_t>(value & 0xFF));
    Wire.write(static_cast<uint8_t>(value >> 8));
  }

  Wire.endTransmission();
}
