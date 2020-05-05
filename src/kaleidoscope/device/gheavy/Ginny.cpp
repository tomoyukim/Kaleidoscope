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

#ifndef KALEIDOSCOPE_VIRTUAL_BUILD
#ifdef ARDUINO_AVR_GINNY

#include "kaleidoscope/Runtime.h"

namespace kaleidoscope {
namespace device {
namespace gheavy {

MCP23018_KEYSCANNER_BOILERPLATE

}
}
}

#endif
#endif // ifndef KALEIDOSCOPE_VIRTUAL_BUILD
