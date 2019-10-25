/* -*- mode: c++ -*-
 * BaseLEDDriver -- LED hardware driver base class for Kaleidoscope
 * Copyright (C) 2019  Keyboard.io, Inc
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#ifndef CRGB
#error cRGB and CRGB *must* be defined before including this header!
#endif

#include "kaleidoscope/driver/BaseLEDDriverBlueprint.h"

namespace kaleidoscope {
namespace driver {

template <typename _LEDDriverBlueprint>
class BaseLEDDriver {
 public:
  BaseLEDDriver() {}

  void setup() {}
  void syncLeds(void) {}
  void setCrgbAt(typename _LEDDriverBlueprint::LEDCountType i, cRGB color) {}
  cRGB getCrgbAt(typename _LEDDriverBlueprint::LEDCountType i) {
    cRGB c = {
      0, 0, 0
    };
    return c;
  }
  typename _LEDDriverBlueprint::LEDCountType getLedIndex(uint8_t key_offset) {
    return -1;
  }
};

}
}
