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

#include <Kaleidoscope.h>
#include <avr/wdt.h>

extern "C" {
#include "kaleidoscope/driver/twi.h"
}

#define ELEMENTS(arr)  (sizeof(arr) / sizeof((arr)[0]))

#define LED_DRIVER_ADDR 0x30

namespace kaleidoscope {
namespace driver {
namespace leddriver {

template <typename _Props>
bool IS31FL3741<_Props>::isLEDChanged = true;
template <typename _Props>
cRGB IS31FL3741<_Props>::led_data[];

template <typename _Props>
void IS31FL3741<_Props>::setup() {
  setAllPwmTo(0xFF);
  selectRegister(LED_REGISTER_CONTROL);
  twiSend(LED_DRIVER_ADDR, 0x01, 0xFF); //global current
  twiSend(LED_DRIVER_ADDR, 0x00, 0x01); //normal operation
}

template <typename _Props>
void IS31FL3741<_Props>::twiSend(uint8_t addr, uint8_t Reg_Add, uint8_t Reg_Dat) {
  uint8_t data[] = {Reg_Add, Reg_Dat };
  uint8_t result = twi_writeTo(addr, data, ELEMENTS(data), 1, 0);
}

template <typename _Props>
void IS31FL3741<_Props>::unlockRegister(void) {
  twiSend(LED_DRIVER_ADDR, CMD_WRITE_ENABLE, WRITE_ENABLE_ONCE); //unlock
}

template <typename _Props>
void IS31FL3741<_Props>::selectRegister(uint8_t page) {
  // Registers automatically get locked at startup and after a given write
  // It'd be nice to disable that.
  unlockRegister();
  twiSend(LED_DRIVER_ADDR, CMD_SET_REGISTER, page);
}

template <typename _Props>
void IS31FL3741<_Props>::setCrgbAt(int8_t i, cRGB crgb) {
  if (i < 0) {
    return;
  }
  if (i < _Props::led_count) {
    cRGB oldColor = getCrgbAt(i);
    isLEDChanged |= !(oldColor.r == crgb.r && oldColor.g == crgb.g && oldColor.b == crgb.b);

    led_data[i] = crgb;
  }
}

template <typename _Props>
int8_t IS31FL3741<_Props>::getLedIndex(uint8_t key_offset) {
  return pgm_read_byte(_Props::key_led_map + key_offset);
}

template <typename _Props>
cRGB IS31FL3741<_Props>::getCrgbAt(int8_t i) {
  if (i < 0 || i > _Props::led_count)
    return {0, 0, 0};

  return led_data[i];
}

template <typename _Props>
void IS31FL3741<_Props>::syncLeds() {
//  if (!isLEDChanged)
//   return;

  uint8_t data[LED_REGISTER_DATA_LARGEST + 1];
  data[0] = 0;// the address of the first byte to copy in
  uint8_t last_led = 0;

  // Write the first LED bank
  selectRegister(LED_REGISTER_DATA0);

  for (auto i = 1; i < LED_REGISTER_DATA0_SIZE; i += 3) {
    data[i] = led_data[last_led].b;
    data[i + 1] = led_data[last_led].g;
    data[i + 2] = led_data[last_led].r;
    last_led++;
  }

  twi_writeTo(LED_DRIVER_ADDR, data, LED_REGISTER_DATA0_SIZE + 1, 1, 0);

  // Don't reset "Last LED", because this is just us picking up from the last bank
  // TODO - we don't use all 117 LEDs on the Imago, so we can probably stop writing earlier
  // Write the second LED bank

  // For space efficiency, we reuse the LED sending buffer
  // The twi library should never send more than the number of elements
  // we say to send it.
  // The page 2 version has 180 elements. The page 3 version has only 171.

  selectRegister(LED_REGISTER_DATA1);

  for (auto i = 1; i < LED_REGISTER_DATA1_SIZE; i += 3) {
    data[i] = led_data[last_led].b;
    data[i + 1] = led_data[last_led].g;
    data[i + 2] = led_data[last_led].r;
    last_led++;
  }

  twi_writeTo(LED_DRIVER_ADDR, data, LED_REGISTER_DATA1_SIZE + 1, 1, 0);

  isLEDChanged = false;
}

template <typename _Props>
void IS31FL3741<_Props>::setAllPwmTo(uint8_t step) {
  selectRegister(LED_REGISTER_PWM0);

  uint8_t data[0xB5] = {};
  data[0] = 0;
  // PWM Register 0 is 0x00 to 0xB3
  for (auto i = 1; i <= 0xB4; i++) {
    data[i] = step;

  }
  twi_writeTo(LED_DRIVER_ADDR, data, 0xB5, 1, 0);

  selectRegister(LED_REGISTER_PWM1);
  // PWM Register 1 is 0x00 to 0xAA
  for (auto i = 1; i <= LED_REGISTER_PWM1_SIZE; i++) {
    data[i] = step;

  }
  twi_writeTo(LED_DRIVER_ADDR, data, 0xAC, 1, 0);
}

}
}
}
