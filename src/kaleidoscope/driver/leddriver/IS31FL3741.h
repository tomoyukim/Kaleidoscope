/* -*- mode: c++ -*-
 * kaleidoscope::driver::leddriver::IS31FL3741 -- LED driver for IS31FL3741 chipsets
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

#pragma once

#include <Arduino.h>

struct cRGB {
  uint8_t b;
  uint8_t g;
  uint8_t r;
};

#define CRGB(r,g,b) (cRGB){b, g, r}

#include "kaleidoscope/driver/leddriver/Base.h"

namespace kaleidoscope {
namespace driver {
namespace leddriver {

struct IS31FL3741Props: public kaleidoscope::driver::leddriver::BaseProps {
  static constexpr uint8_t NOLED = 254;
  static constexpr uint8_t *key_led_map = {};
};

template <typename _LEDDriverProps>
class IS31FL3741: public kaleidoscope::driver::leddriver::Base<_LEDDriverProps> {
 public:

  static void setup();
  static void syncLeds();
  static void setCrgbAt(int8_t i, cRGB crgb);
  static cRGB getCrgbAt(int8_t i);
  static int8_t getLedIndex(uint8_t key_offset);

  static cRGB led_data[117]; // 117 is the number of LEDs the chip drives
  // until we clean stuff up a bit, it's easiest to just have the whole struct around

 private:
  static bool isLEDChanged;
  static void selectRegister(uint8_t);
  static void unlockRegister();
  static void setAllPwmTo(uint8_t);
  static void twiSend(uint8_t addr, uint8_t Reg_Add, uint8_t Reg_Dat);

  static constexpr uint8_t CMD_SET_REGISTER = 0xFD;
  static constexpr uint8_t CMD_WRITE_ENABLE = 0xFE;
  static constexpr uint8_t WRITE_ENABLE_ONCE = 0b11000101;

  static constexpr uint8_t LED_REGISTER_PWM0 = 0x00;
  static constexpr uint8_t LED_REGISTER_PWM1 = 0x01;
  static constexpr uint8_t LED_REGISTER_DATA0 = 0x02;
  static constexpr uint8_t LED_REGISTER_DATA1 = 0x03;
  static constexpr uint8_t LED_REGISTER_CONTROL = 0x04;

  static constexpr uint8_t LED_REGISTER_PWM0_SIZE = 0xB4;
  static constexpr uint8_t LED_REGISTER_PWM1_SIZE = 0xAB;
  static constexpr uint8_t LED_REGISTER_DATA0_SIZE = 0xB4;
  static constexpr uint8_t LED_REGISTER_DATA1_SIZE = 0xAB;

  static constexpr uint8_t LED_REGISTER_DATA_LARGEST = LED_REGISTER_DATA0_SIZE;
};

}
}
}
