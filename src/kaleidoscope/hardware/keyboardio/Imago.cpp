/* -*- mode: c++ -*-
 * Kaleidoscope-Hardware-Keyboardio-Imago -- Imago hardware support for Kaleidoscope
 * Copyright (C) 2018, 2019  Keyboard.io, Inc
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of version 3 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef ARDUINO_AVR_KEYBOARDIO_IMAGO

#include <Kaleidoscope.h>
#include <avr/wdt.h>

extern "C" {
#include "kaleidoscope/driver/twi.h"
}

namespace kaleidoscope {
namespace hardware {
namespace keyboardio {

ATMEGA_KEYSCANNER_IMPLEMENTATION();

void Imago::setup() {
  digitalWrite(MOSI, HIGH);
  digitalWrite(SS, HIGH);
  uint8_t twi_uninitialized = 1;
  if (twi_uninitialized--) {
    twi_init();
  }
  TWBR = 10;

  kaleidoscope::hardware::avr::AVRDevice<ImagoDeviceProps>::setup();
}

}
}
}

HARDWARE_IMPLEMENTATION KeyboardHardware;

#endif
