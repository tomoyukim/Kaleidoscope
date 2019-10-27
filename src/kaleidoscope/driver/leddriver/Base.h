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

namespace kaleidoscope {
namespace driver {
namespace leddriver {

struct BaseProps {
  typedef int8_t LEDCountType;
  static constexpr LEDCountType led_count = 0;
};

template <typename _LEDDriverProps>
class Base {
 public:
  Base() {}

  void setup() {}
  void syncLeds(void) {}
  void setCrgbAt(typename _LEDDriverProps::LEDCountType i, cRGB color) {}
  cRGB getCrgbAt(typename _LEDDriverProps::LEDCountType i) {
    cRGB c = {
      0, 0, 0
    };
    return c;
  }
  typename _LEDDriverProps::LEDCountType getLedIndex(uint8_t key_offset) {
    return -1;
  }

 protected:
  typedef _LEDDriverProps Props_;
};

}
}
}
