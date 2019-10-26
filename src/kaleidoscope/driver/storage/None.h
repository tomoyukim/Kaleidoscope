/* -*- mode: c++ -*-
 * NoStorage -- Dummy Storage hardware driver for Kaleidoscope
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

#include "kaleidoscope/driver/storage/Base.h"

namespace kaleidoscope {
namespace driver {
namespace storage {

struct NoProps : public kaleidoscope::driver::storage::BaseProps {};

/*
 * The purpose of this class (and the accompanying Props) is to serve as a
 * default for the base `DeviceProps` class, with a name more descriptive than
 * `Base`. In practice, one shouldn't use it, and should override the it in the
 * device blueprint.
 */
class None : public kaleidoscope::driver::storage::Base<NoProps> {};

}
}
}
