#pragma once

#include <Arduino.h>

namespace SegmentFont {

using Mask = uint16_t;

enum class Style : uint8_t {
  Standard,
  Fancy
};

// HT16K33/VK16K33 ROW output mapping from the 15-segment PCB schematic.
constexpr Mask SegA  = 0x0001; // ROW0
constexpr Mask SegB  = 0x0002; // ROW1
constexpr Mask SegC  = 0x0004; // ROW2
constexpr Mask SegD  = 0x0008; // ROW3
constexpr Mask SegE  = 0x0010; // ROW4
constexpr Mask SegF  = 0x0020; // ROW5
constexpr Mask SegG1 = 0x0040; // ROW6
constexpr Mask SegG2 = 0x0080; // ROW7
constexpr Mask SegH  = 0x0100; // ROW8
constexpr Mask SegJ  = 0x0200; // ROW9
constexpr Mask SegK  = 0x0400; // ROW10
constexpr Mask SegL  = 0x0800; // ROW11 - SWAP with L
constexpr Mask SegM  = 0x1000; // ROW12
constexpr Mask SegN  = 0x2000; // ROW13 - SWAP with N
constexpr Mask SegDP = 0x4000; // ROW14

constexpr Mask SegG = SegG1 | SegG2;

Mask encode(char c, bool decimalPoint, Style style = Style::Standard);
Mask fromSevenSegment(uint8_t mask);

}
