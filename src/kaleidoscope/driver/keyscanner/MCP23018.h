/* -*- mode: c++ -*-
 * kaleidoscope::driver::keyscanner::MCP23018 -- AVR ATmega + MCP23018-based keyscanner component
 * Copyright (C) 2018-2020  Keyboard.io, Inc
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

#include <Arduino.h>

#include "kaleidoscope/macro_helpers.h"
#include "kaleidoscope/driver/keyscanner/ATmega.h"
#include "kaleidoscope/driver/i2c/i2cmaster.h"

#define I2C_ADDR        0b0100000
#define I2C_ADDR_WRITE  ( (I2C_ADDR<<1) | I2C_WRITE )
#define I2C_ADDR_READ   ( (I2C_ADDR<<1) | I2C_READ  )
#define IODIRA          0x00
#define IODIRB          0x01
#define GPPUA           0x0C
#define GPPUB           0x0D
#define GPIOA           0x12
#define GPIOB           0x13
#define OLATA           0x14
#define OLATB           0x15

#ifndef KALEIDOSCOPE_VIRTUAL_BUILD
#define MCP23018_KEYSCANNER_PROPS(ROW_PINS_, COL_PINS_, MCP_ROWS_, OFFSET_) \
  KEYSCANNER_PROPS(NUM_ARGS(ROW_PINS_) + MCP_ROWS_, NUM_ARGS(COL_PINS_));   \
  static constexpr uint8_t atmega_rows = NUM_ARGS(ROW_PINS_);               \
  static constexpr uint8_t atmega_columns = NUM_ARGS(COL_PINS_);            \
  static constexpr uint8_t matrix_row_pins[atmega_rows] = ROW_PINS_;                \
  static constexpr uint8_t matrix_col_pins[atmega_columns] = COL_PINS_;             \
  static constexpr uint8_t atmega_row_offset = OFFSET_;
#else // ifndef KALEIDOSCOPE_VIRTUAL_BUILD
#define MCP23018_KEYSCANNER_PROPS(ROW_PINS_, COL_PINS_, MCP_ROWS_, OFFSET_) \
  KEYSCANNER_PROPS(NUM_ARGS(ROW_PINS_) + MCP_ROWS_, NUM_ARGS(COL_PINS_));
#endif // ifndef KALEIDOSCOPE_VIRTUAL_BUILD

#define MCP23018_KEYSCANNER_BOILERPLATE                                                                   \
  KEYSCANNER_PROPS_BOILERPLATE(kaleidoscope::Device::KeyScannerProps);                                  \
  constexpr uint8_t kaleidoscope::Device::KeyScannerProps::matrix_row_pins[atmega_rows];                \
  constexpr uint8_t kaleidoscope::Device::KeyScannerProps::matrix_col_pins[atmega_columns];             \
  template<>                                                                                            \
  uint16_t kaleidoscope::Device::KeyScanner::previousKeyState_[kaleidoscope::Device::KeyScannerProps::matrix_rows] = {}; \
  template<>                                                                                            \
  uint16_t kaleidoscope::Device::KeyScanner::keyState_[kaleidoscope::Device::KeyScannerProps::matrix_rows] = {};         \
  template<>                                                                                            \
  uint16_t kaleidoscope::Device::KeyScanner::masks_[kaleidoscope::Device::KeyScannerProps::matrix_rows] = {};            \
  template<>                                                                               \
  uint8_t kaleidoscope::Device::KeyScanner::debounce_matrix_[kaleidoscope::Device::KeyScannerProps::matrix_rows][kaleidoscope::Device::KeyScannerProps::matrix_columns] = {}; \
                                                                                           \
  template<> \
  uint8_t kaleidoscope::Device::KeyScanner::mcp23018_status_ = 0x20; \
  \
  ISR(TIMER1_OVF_vect) {                                                                   \
    Runtime.device().keyScanner().do_scan_ = true;                                         \
  }

namespace kaleidoscope {
namespace driver {
namespace keyscanner {

struct MCP23018Props: kaleidoscope::driver::keyscanner::ATmegaProps {};

#ifndef KALEIDOSCOPE_VIRTUAL_BUILD
template <typename _KeyScannerProps>
class MCP23018: public kaleidoscope::driver::keyscanner::ATmega<_KeyScannerProps> {
 private:
  typedef MCP23018<_KeyScannerProps> ThisType;

 public:
  void setup() {
    ATmega<_KeyScannerProps>::setup();

    mcp23018_status_ = initMCP23018();
  }

  // TODO: private
  uint8_t readMCP23018Cols(uint8_t row) {
    if (mcp23018_status_) {
      return 0;
    }
    uint8_t data = 0;

    mcp23018_status_ = i2c_start(I2C_ADDR_WRITE);
    if (mcp23018_status_)
      goto out;
    mcp23018_status_ = i2c_write(GPIOB);
    if (mcp23018_status_)
      goto out;
    mcp23018_status_ = i2c_start(I2C_ADDR_READ);
    if (mcp23018_status_)
      goto out;

    data = i2c_readNak();
    data = ~data;
 out:
    i2c_stop();
    return data;
  }

  void __attribute__((optimize(3))) readRow(uint8_t current_row) {
    const uint8_t atmega_row_offset = _KeyScannerProps::atmega_row_offset;

    if (current_row >= atmega_row_offset) {
      ATmega<_KeyScannerProps>::readRow(current_row);
    } else {
      // Select the row on the MCP23018
      if (mcp23018_status_) {
        // do nothing on error
      } else {
        mcp23018_status_ = i2c_start(I2C_ADDR_WRITE);
        if (mcp23018_status_)
          goto out;
        mcp23018_status_ = i2c_write(GPIOA);
        if (mcp23018_status_)
          goto out;
        mcp23018_status_ = i2c_write(0xFF & ~(1 << current_row));
        if (mcp23018_status_)
          goto out;
     out:
        i2c_stop();
      }

      // Read cols
      uint16_t mask, cols;

      mask = ATmega<_KeyScannerProps>::debounceMaskForRow(current_row);
      cols = (ATmega<_KeyScannerProps>::readCols() & mask) |
             (ATmega<_KeyScannerProps>::keyState_[current_row] & ~mask);
      ATmega<_KeyScannerProps>::debounceRow(cols ^ ATmega<_KeyScannerProps>::keyState_[current_row], current_row);
      ATmega<_KeyScannerProps>::keyState_[current_row] = cols;
    }
  }

  void __attribute__((optimize(3))) readMatrix(void) {
    // TODO: MCP reset loop
    for (uint8_t current_row = 0; current_row < _KeyScannerProps::matrix_rows; current_row++) {
      readRow(current_row);
    }
  }

 private:
  static uint8_t mcp23018_status_;
  typedef _KeyScannerProps KeyScannerProps_;
  static uint16_t previousKeyState_[_KeyScannerProps::matrix_rows];
  static uint16_t keyState_[_KeyScannerProps::matrix_rows];
  static uint16_t masks_[_KeyScannerProps::matrix_rows];
  static uint8_t debounce_matrix_[_KeyScannerProps::matrix_rows][_KeyScannerProps::matrix_columns];

  static uint8_t initMCP23018() {
    uint8_t status = 0x20;

    i2c_init();
    delay(1000);

    status = i2c_start(I2C_ADDR_WRITE);
    if (status)
      goto out;
    status = i2c_write(IODIRA);
    if (status)
      goto out;
    status = i2c_write(0b00000000);
    if (status)
      goto out;
    status = i2c_write(0b00111111);
    if (status)
      goto out;

    i2c_stop();

    status = i2c_start(I2C_ADDR_WRITE);
    if (status)
      goto out;
    status = i2c_write(GPPUA);
    if (status)
      goto out;
    status = i2c_write(0b00000000);
    if (status)
      goto out;
    status = i2c_write(0b00111111);
    if (status)
      goto out;

 out:
    i2c_stop();

    return status;
  }
};
#else // ifndef KALEIDOSCOPE_VIRTUAL_BUILD
template <typename _KeyScannerProps>
class ATmega : public keyscanner::None {};
#endif // ifndef KALEIDOSCOPE_VIRTUAL_BUILD

}
}
}
