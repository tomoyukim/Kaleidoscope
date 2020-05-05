/* -*- mode: c++ -*-
 * kaleidoscope::device::gheavy::Ginny -- Ginny hardware support for Kaleidoscope
 * Copyright (C) 2020  Keyboard.io, Inc
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#ifdef ARDUINO_AVR_GINNY

#define KALEIDOSCOPE_BOOTLOADER_FLIP_WORKAROUND 1

#include <Arduino.h>

#include "kaleidoscope/device/Base.h"

#include "kaleidoscope/device/ATmega32U4Keyboard.h"
#include "kaleidoscope/driver/bootloader/avr/FLIP.h"
#include "kaleidoscope/driver/keyscanner/MCP23018.h"

namespace kaleidoscope {
namespace device {
namespace gheavy {

struct GinnyProps: kaleidoscope::device::ATmega32U4KeyboardProps {
  struct KeyScannerProps: public kaleidoscope::driver::keyscanner::MCP23018Props {
    MCP23018_KEYSCANNER_PROPS(
        ROW_PIN_LIST({PIN_B0, PIN_B1, PIN_B2, PIN_B3, PIN_C6, PIN_D2, PIN_D3}),
        COL_PIN_LIST({PIN_F1, PIN_F2, PIN_F3, PIN_F4}),
        7,
        7
    );
  };
  typedef kaleidoscope::driver::keyscanner::MCP23018<KeyScannerProps> KeyScanner;
  typedef kaleidoscope::driver::bootloader::avr::FLIP BootLoader;
  static constexpr const char *short_name = "ginny";
};

#ifndef KALEIDOSCOPE_VIRTUAL_BUILD
class Ginny: public kaleidoscope::device::ATmega32U4Keyboard<GinnyProps> {
 public:
  Ginny() {
    mcu_.disableJTAG();
  }
};
#else // ifndef KALEIDOSCOPE_VIRTUAL_BUILD
class Ginny;
#endif // ifndef KALEIDOSCOPE_VIRTUAL_BUILD

#define PER_KEY_DATA(dflt,                                                       \
   r0c0 ,r0c1 ,r0c2 ,r0c3 ,r0c4 ,r0c5      ,r0c6 ,r0c7 ,r0c8 ,r0c9 ,r0c10 ,r0c11 \
  ,r1c0 ,r1c1 ,r1c2 ,r1c3 ,r1c4 ,r1c5      ,r1c6 ,r1c7 ,r1c8 ,r1c9 ,r1c10 ,r1c11 \
                    ,r2c0 ,r2c1 ,r2c2      ,r2c3 ,r2c4 ,r2c5                     \
) \
  \
   dflt ,r0c6  ,r1c6  ,r2c4 \
  ,dflt ,r0c7  ,r1c7  ,r2c3 \
  ,dflt ,r0c8  ,r1c8  ,r2c5 \
  ,dflt ,r0c9  ,r1c9  ,dflt \
  ,dflt ,r0c10 ,r1c10 ,dflt \
  ,dflt ,r0c11 ,r1c11 ,dflt \
  ,dflt ,dflt  ,dflt  ,dflt \
  \
  ,dflt ,r0c5  ,r1c5  ,r2c2 \
  ,dflt ,r0c4  ,r1c4  ,r2c1 \
  ,dflt ,r0c3  ,r1c3  ,r2c0 \
  ,dflt ,r0c2  ,r1c2  ,dflt \
  ,dflt ,r0c1  ,r1c1  ,dflt \
  ,dflt ,r0c0  ,r1c0  ,dflt \
  ,dflt ,dflt  ,dflt  ,dflt

#define PER_KEY_DATA_STACKED(dflt, \
   r0c0 ,r0c1 ,r0c2 ,r0c3 ,r0c4 ,r0c5 \
  ,r1c0 ,r1c1 ,r1c2 ,r1c3 ,r1c4 ,r1c5 \
                   ,r2c0 ,r2c1 ,r2c2 \
  \
  ,r0c6 ,r0c7 ,r0c8 ,r0c9 ,r0c10 ,r0c11 \
  ,r1c6 ,r1c7 ,r1c8 ,r1c9 ,r1c10 ,r1c11 \
                   ,r2c3 ,r2c4  ,r2c5 \
                   ) \
                   \
   dflt ,r0c6  ,r1c6  ,r2c4 \
  ,dflt ,r0c7  ,r1c7  ,r2c3 \
  ,dflt ,r0c8  ,r1c8  ,r2c5 \
  ,dflt ,r0c9  ,r1c9  ,dflt \
  ,dflt ,r0c10 ,r1c10 ,dflt \
  ,dflt ,r0c11 ,r1c11 ,dflt \
  ,dflt ,dflt  ,dflt  ,dflt \
  \
  ,dflt ,r0c5  ,r1c5  ,r2c2 \
  ,dflt ,r0c4  ,r1c4  ,r2c1 \
  ,dflt ,r0c3  ,r1c3  ,r2c0 \
  ,dflt ,r0c2  ,r1c2  ,dflt \
  ,dflt ,r0c1  ,r1c1  ,dflt \
  ,dflt ,r0c0  ,r1c0  ,dflt \
  ,dflt ,dflt  ,dflt  ,dflt

}
}

EXPORT_DEVICE(kaleidoscope::device::gheavy::Ginny)

}

#endif
