/******************************************************************************
 * AlphaScore
 * ----------------------------------------------------------------------------
 * File:
 *     SegmentFont.cpp
 *
 * Description:
 *     Implements character and 7-segment conversion for the 15-segment displays.
 *
 * Copyright (c) 2026 Michal Podlejski
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include "SegmentFont.h"

namespace SegmentFont {
namespace {

// Character masks generated from Mission Pinball Framework segment_mappings.py.
// Standard uses MPF SevenSegments; Fancy uses MPF FourteenSegments.
Mask encodeStandard(uint8_t code) {
  switch (code) {
    case 32: return 0;
    case 33: return SegB | SegC | SegDP;
    case 34: return SegB | SegF;
    case 35: return SegB | SegC | SegD | SegE | SegF | SegG1 | SegG2;
    case 36: return SegA | SegC | SegD | SegF | SegG1 | SegG2;
    case 37: return SegB | SegE | SegG1 | SegG2 | SegDP;
    case 38: return SegB | SegC | SegG1 | SegG2;
    case 39: return SegF;
    case 40: return SegA | SegD | SegF;
    case 41: return SegA | SegB | SegD;
    case 42: return SegA | SegF;
    case 43: return SegE | SegF | SegG1 | SegG2;
    case 44: return SegE;
    case 45: return SegG1 | SegG2;
    case 46: return SegDP;
    case 47: return SegB | SegE | SegG1 | SegG2;
    case 48: return SegA | SegB | SegC | SegD | SegE | SegF;
    case 49: return SegB | SegC;
    case 50: return SegA | SegB | SegD | SegE | SegG1 | SegG2;
    case 51: return SegA | SegB | SegC | SegD | SegG1 | SegG2;
    case 52: return SegB | SegC | SegF | SegG1 | SegG2;
    case 53: return SegA | SegC | SegD | SegF | SegG1 | SegG2;
    case 54: return SegA | SegC | SegD | SegE | SegF | SegG1 | SegG2;
    case 55: return SegA | SegB | SegC;
    case 56: return SegA | SegB | SegC | SegD | SegE | SegF | SegG1 | SegG2;
    case 57: return SegA | SegB | SegC | SegD | SegF | SegG1 | SegG2;
    case 58: return SegA | SegD;
    case 59: return SegA | SegC | SegD;
    case 60: return SegA | SegF | SegG1 | SegG2;
    case 61: return SegD | SegG1 | SegG2;
    case 62: return SegA | SegB | SegG1 | SegG2;
    case 63: return SegA | SegB | SegE | SegG1 | SegG2 | SegDP;
    case 64: return SegA | SegB | SegC | SegD | SegE | SegG1 | SegG2;
    case 65: return SegA | SegB | SegC | SegE | SegF | SegG1 | SegG2;
    case 66: return SegC | SegD | SegE | SegF | SegG1 | SegG2;
    case 67: return SegA | SegD | SegE | SegF;
    case 68: return SegB | SegC | SegD | SegE | SegG1 | SegG2;
    case 69: return SegA | SegD | SegE | SegF | SegG1 | SegG2;
    case 70: return SegA | SegE | SegF | SegG1 | SegG2;
    case 71: return SegA | SegC | SegD | SegE | SegF;
    case 72: return SegB | SegC | SegE | SegF | SegG1 | SegG2;
    case 73: return SegE | SegF;
    case 74: return SegB | SegC | SegD | SegE;
    case 75: return SegA | SegC | SegE | SegF | SegG1 | SegG2;
    case 76: return SegD | SegE | SegF;
    case 77: return SegA | SegC | SegE;
    case 78: return SegA | SegB | SegC | SegE | SegF;
    case 79: return SegA | SegB | SegC | SegD | SegE | SegF;
    case 80: return SegA | SegB | SegE | SegF | SegG1 | SegG2;
    case 81: return SegA | SegB | SegD | SegF | SegG1 | SegG2;
    case 82: return SegA | SegB | SegE | SegF;
    case 83: return SegA | SegC | SegD | SegF | SegG1 | SegG2;
    case 84: return SegD | SegE | SegF | SegG1 | SegG2;
    case 85: return SegB | SegC | SegD | SegE | SegF;
    case 86: return SegB | SegC | SegD | SegE | SegF;
    case 87: return SegB | SegD | SegF;
    case 88: return SegB | SegC | SegE | SegF | SegG1 | SegG2;
    case 89: return SegB | SegC | SegD | SegF | SegG1 | SegG2;
    case 90: return SegA | SegB | SegD | SegE | SegG1 | SegG2;
    case 91: return SegA | SegD | SegE | SegF;
    case 92: return SegC | SegF | SegG1 | SegG2;
    case 93: return SegA | SegB | SegC | SegD;
    case 94: return SegA | SegB | SegF;
    case 95: return SegD;
    case 96: return SegB;
    case 97: return SegA | SegB | SegC | SegD | SegE | SegG1 | SegG2;
    case 98: return SegC | SegD | SegE | SegF | SegG1 | SegG2;
    case 99: return SegD | SegE | SegG1 | SegG2;
    case 100: return SegB | SegC | SegD | SegE | SegG1 | SegG2;
    case 101: return SegA | SegB | SegD | SegE | SegF | SegG1 | SegG2;
    case 102: return SegA | SegE | SegF | SegG1 | SegG2;
    case 103: return SegA | SegB | SegC | SegD | SegF | SegG1 | SegG2;
    case 104: return SegC | SegE | SegF | SegG1 | SegG2;
    case 105: return SegE;
    case 106: return SegC | SegD;
    case 107: return SegA | SegC | SegE | SegF | SegG1 | SegG2;
    case 108: return SegE | SegF;
    case 109: return SegC | SegE;
    case 110: return SegC | SegE | SegG1 | SegG2;
    case 111: return SegC | SegD | SegE | SegG1 | SegG2;
    case 112: return SegA | SegB | SegE | SegF | SegG1 | SegG2;
    case 113: return SegA | SegB | SegC | SegF | SegG1 | SegG2;
    case 114: return SegE | SegG1 | SegG2;
    case 115: return SegA | SegC | SegD | SegF | SegG1 | SegG2;
    case 116: return SegD | SegE | SegF | SegG1 | SegG2;
    case 117: return SegC | SegD | SegE;
    case 118: return SegC | SegD | SegE;
    case 119: return SegC | SegE;
    case 120: return SegB | SegC | SegE | SegF | SegG1 | SegG2;
    case 121: return SegB | SegC | SegD | SegF | SegG1 | SegG2;
    case 122: return SegA | SegB | SegD | SegE | SegG1 | SegG2;
    case 123: return SegB | SegC | SegG1 | SegG2;
    case 124: return SegE | SegF;
    case 125: return SegE | SegF | SegG1 | SegG2;
    case 126: return SegA;
    default: return 0;
  }
}

Mask encodeFancy(uint8_t code) {
  switch (code) {
    case 32: return 0;
    case 33: return SegB | SegC | SegDP;
    case 34: return SegB | SegJ;
    case 35: return SegB | SegC | SegD | SegG1 | SegG2 | SegJ | SegM;
    case 36: return SegA | SegC | SegD | SegF | SegG1 | SegG2 | SegJ | SegM;
    case 37: return SegC | SegF | SegG1 | SegG2 | SegH | SegK | SegL | SegN;
    case 38: return SegA | SegD | SegE | SegG1 | SegH | SegJ | SegL;
    case 39: return SegJ;
    case 40: return SegK | SegL;
    case 41: return SegH | SegN;
    case 42: return SegG1 | SegG2 | SegH | SegJ | SegK | SegL | SegM | SegN;
    case 43: return SegG1 | SegG2 | SegJ | SegM;
    case 44: return SegN;
    case 45: return SegG1 | SegG2;
    case 46: return SegDP;
    case 47: return SegK | SegN;
    case 48: return SegA | SegB | SegC | SegD | SegE | SegF | SegK | SegN;
    case 49: return SegB | SegC | SegK;
    case 50: return SegA | SegB | SegD | SegE | SegG1 | SegG2;
    case 51: return SegA | SegB | SegC | SegD | SegG2;
    case 52: return SegB | SegC | SegF | SegG1 | SegG2;
    case 53: return SegA | SegD | SegF | SegG1 | SegL;
    case 54: return SegA | SegC | SegD | SegE | SegF | SegG1 | SegG2;
    case 55: return SegA | SegB | SegC;
    case 56: return SegA | SegB | SegC | SegD | SegE | SegF | SegG1 | SegG2;
    case 57: return SegA | SegB | SegC | SegD | SegF | SegG1 | SegG2;
    case 58: return SegJ | SegM;
    case 59: return SegJ | SegN;
    case 60: return SegG1 | SegK | SegL;
    case 61: return SegD | SegG1 | SegG2;
    case 62: return SegG2 | SegH | SegN;
    case 63: return SegA | SegB | SegG2 | SegM | SegDP;
    case 64: return SegA | SegB | SegC | SegD | SegE | SegG1 | SegM;
    case 65: return SegA | SegB | SegC | SegE | SegF | SegG1 | SegG2;
    case 66: return SegA | SegB | SegC | SegD | SegG2 | SegJ | SegM;
    case 67: return SegA | SegD | SegE | SegF;
    case 68: return SegA | SegB | SegC | SegD | SegJ | SegM;
    case 69: return SegA | SegD | SegE | SegF | SegG1;
    case 70: return SegA | SegE | SegF | SegG1;
    case 71: return SegA | SegC | SegD | SegE | SegF | SegG2;
    case 72: return SegB | SegC | SegE | SegF | SegG1 | SegG2;
    case 73: return SegA | SegD | SegJ | SegM;
    case 74: return SegB | SegC | SegD | SegE;
    case 75: return SegE | SegF | SegG1 | SegK | SegL;
    case 76: return SegD | SegE | SegF;
    case 77: return SegB | SegC | SegE | SegF | SegH | SegK;
    case 78: return SegB | SegC | SegE | SegF | SegH | SegL;
    case 79: return SegA | SegB | SegC | SegD | SegE | SegF;
    case 80: return SegA | SegB | SegE | SegF | SegG1 | SegG2;
    case 81: return SegA | SegB | SegC | SegD | SegE | SegF | SegL;
    case 82: return SegA | SegB | SegE | SegF | SegG1 | SegG2 | SegL;
    case 83: return SegA | SegC | SegD | SegF | SegG1 | SegG2;
    case 84: return SegA | SegJ | SegM;
    case 85: return SegB | SegC | SegD | SegE | SegF;
    case 86: return SegE | SegF | SegK | SegN;
    case 87: return SegB | SegC | SegE | SegF | SegL | SegN;
    case 88: return SegH | SegK | SegL | SegN;
    case 89: return SegB | SegC | SegD | SegF | SegG1 | SegG2;
    case 90: return SegA | SegD | SegK | SegN;
    case 91: return SegA | SegD | SegE | SegF;
    case 92: return SegH | SegL;
    case 93: return SegA | SegB | SegC | SegD;
    case 94: return SegL | SegN;
    case 95: return SegD;
    case 96: return SegH;
    case 97: return SegD | SegE | SegG1 | SegM;
    case 98: return SegD | SegE | SegF | SegG1 | SegL;
    case 99: return SegD | SegE | SegG1 | SegG2;
    case 100: return SegB | SegC | SegD | SegG2 | SegN;
    case 101: return SegD | SegE | SegG1 | SegN;
    case 102: return SegG1 | SegG2 | SegK | SegM;
    case 103: return SegA | SegB | SegC | SegD | SegG2 | SegH;
    case 104: return SegC | SegE | SegF | SegG1 | SegG2;
    case 105: return SegM;
    case 106: return SegE | SegJ | SegN;
    case 107: return SegJ | SegK | SegL | SegM;
    case 108: return SegJ | SegM;
    case 109: return SegC | SegE | SegG1 | SegG2 | SegM;
    case 110: return SegC | SegE | SegG1 | SegG2;
    case 111: return SegC | SegD | SegE | SegG1 | SegG2;
    case 112: return SegA | SegE | SegF | SegG1 | SegK;
    case 113: return SegA | SegB | SegC | SegG2 | SegH;
    case 114: return SegE | SegG1 | SegG2;
    case 115: return SegD | SegG2 | SegL;
    case 116: return SegD | SegE | SegF | SegG1;
    case 117: return SegC | SegD | SegE;
    case 118: return SegE | SegN;
    case 119: return SegC | SegD | SegE | SegM;
    case 120: return SegH | SegK | SegL | SegN;
    case 121: return SegB | SegC | SegD | SegG2 | SegJ;
    case 122: return SegD | SegG1 | SegN;
    case 123: return SegA | SegD | SegG1 | SegH | SegN;
    case 124: return SegJ | SegM;
    case 125: return SegA | SegD | SegG2 | SegK | SegL;
    case 126: return SegG1 | SegG2 | SegK | SegN;
    default: return 0;
  }
}

}

Mask encode(char c, bool decimalPoint, Style style) {
  const uint8_t code = static_cast<uint8_t>(c);
  Mask out = style == Style::Fancy ? encodeFancy(code) : encodeStandard(code);

  if (decimalPoint) {
    out |= SegDP;
  }

  return out;
}

Mask fromSevenSegment(uint8_t mask) {
  Mask out = 0;

  if (mask & 0x01) out |= SegA;
  if (mask & 0x02) out |= SegB;
  if (mask & 0x04) out |= SegC;
  if (mask & 0x08) out |= SegD;
  if (mask & 0x10) out |= SegE;
  if (mask & 0x20) out |= SegF;
  if (mask & 0x40) out |= SegG;
  if (mask & 0x80) out |= SegDP;

  return out;
}

}




