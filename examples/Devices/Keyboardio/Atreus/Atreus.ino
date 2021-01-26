/* -*- mode: c++ -*-
 * Atreus -- Chrysalis-enabled Sketch for the Keyboardio Atreus
 * Copyright (C) 2018, 2019  Keyboard.io, Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef BUILD_INFORMATION
#define BUILD_INFORMATION "t7m.keymap.20210126"
#endif

#include "Kaleidoscope.h"
#include "Kaleidoscope-FocusSerial.h"
#include "Kaleidoscope-Macros.h"
#include "Kaleidoscope-Qukeys.h"
#include "Kaleidoscope-TapDance.h"


#define MO(n) ShiftToLayer(n)
#define TG(n) LockLayer(n)

enum {
  MACRO_QWERTY,
  MACRO_VERSION_INFO
};

#define Key_Exclamation LSHIFT(Key_1)
#define Key_At LSHIFT(Key_2)
#define Key_Hash LSHIFT(Key_3)
#define Key_Dollar LSHIFT(Key_4)
#define Key_Percent LSHIFT(Key_5)
#define Key_Caret LSHIFT(Key_6)
#define Key_And LSHIFT(Key_7)
#define Key_Star LSHIFT(Key_8)
#define Key_Plus LSHIFT(Key_Equals)

enum {
  QWERTY,
  FUN,
  UPPER
};

const int ei_kana = 0;

/* *INDENT-OFF* */
KEYMAPS(
  [QWERTY] = KEYMAP_STACKED
  (
       Key_Q   ,Key_W        ,Key_E         ,Key_R         ,Key_T
      ,Key_A   ,Key_S        ,Key_D         ,Key_F         ,Key_G
      ,Key_Z   ,Key_X        ,Key_C         ,Key_V         ,Key_B           ,Key_Tab
      ,Key_Esc ,Key_Backtick ,Key_LeftAlt   ,TD(ei_kana)   ,Key_LeftControl ,Key_LeftShift

                      ,Key_Y     ,Key_U      ,Key_I          ,Key_O         ,Key_P
                      ,Key_H     ,Key_J      ,Key_K          ,Key_L         ,Key_Semicolon
       ,Key_Backspace ,Key_N     ,Key_M      ,Key_Comma      ,Key_Period    ,Key_Slash
       ,Key_Enter     ,Key_Space ,Key_Equals ,Key_Minus      ,Key_Quote     ,Key_Backslash
  ),

  [FUN] = KEYMAP_STACKED
  (
       Key_Exclamation  ,Key_At           ,Key_Hash        ,Key_Dollar          ,Key_Percent
      ,Key_Caret        ,Key_And          ,Key_Star        ,Key_LeftParen       ,Key_RightParen
      ,XXX              ,Key_LeftBracket  ,Key_RightBracket     ,Key_LeftCurlyBracket  ,Key_RightCurlyBracket ,___
      ,___              ,___              ,___                  ,___                   ,___                   ,___

                    ,Key_Slash     ,Key_7      ,Key_8         ,Key_9     ,Key_Minus
                    ,Key_Star      ,Key_4      ,Key_5         ,Key_6     ,Key_Plus
      ,___          ,Key_0         ,Key_1      ,Key_2         ,Key_3     ,Key_Equals
      ,___          ,___           ,___           ,___              ,___             ,___
   ),

  [UPPER] = KEYMAP_STACKED
  (
       Key_Insert                ,Key_Home         ,Key_UpArrow     ,Key_End           ,Key_PageUp
      ,M(MACRO_VERSION_INFO)     ,Key_LeftArrow    ,Key_DownArrow   ,Key_RightArrow    ,Key_PageDown
      ,XXX                       ,XXX              ,XXX             ,XXX               ,XXX           ,___
      ,___                       ,___              ,___             ,___               ,___           ,___

                  ,Key_F12       ,Key_F7          ,Key_F8          ,Key_F9         ,Key_PrintScreen
                  ,Key_F11       ,Key_F4          ,Key_F5          ,Key_F6         ,Key_ScrollLock
      ,Key_Delete ,Key_F10       ,Key_F1          ,Key_F2          ,Key_F3         ,XXX
      ,___        ,___           ,___             ,___             ,___            ,___
   )
)
/* *INDENT-ON* */

void tapDanceAction(uint8_t tap_dance_index, KeyAddr key_addr, uint8_t tap_count, kaleidoscope::plugin::TapDance::ActionType tap_dance_action) {
  switch (tap_dance_index) {
  case ei_kana: // Mac Eng/Jpn switch
    return tapDanceActionKeys(tap_count, tap_dance_action, Key_Lang2, Key_Lang1);
  }
}

KALEIDOSCOPE_INIT_PLUGINS(
  Qukeys,
  Focus,
  Macros,
  TapDance
);

const macro_t *macroAction(uint8_t macroIndex, uint8_t keyState) {
  switch (macroIndex) {
  case MACRO_QWERTY:
    // This macro is currently unused, but is kept around for compatibility
    // reasons. We used to use it in place of `MoveToLayer(QWERTY)`, but no
    // longer do. We keep it so that if someone still has the old layout with
    // the macro in EEPROM, it will keep working after a firmware update.
    Layer.move(QWERTY);
    break;
  case MACRO_VERSION_INFO:
    if (keyToggledOn(keyState)) {
      Macros.type(PSTR("Keyboardio Atreus - Kaleidoscope "));
      Macros.type(PSTR(BUILD_INFORMATION));
    }
    break;
  default:
    break;
  }

  return MACRO_NONE;
}

void setup() {
  QUKEYS(
    // L
    kaleidoscope::plugin::Qukey(QWERTY, KeyAddr(3, 0), MO(UPPER)),
    kaleidoscope::plugin::Qukey(QWERTY, KeyAddr(3, 3), Key_LeftGui),
//    kaleidoscope::plugin::Qukey(QWERTY, KeyAddr(3, 4), Key_LeftControl),
//    kaleidoscope::plugin::Qukey(QWERTY, KeyAddr(3, 5), Key_LeftShift),
    // R
    kaleidoscope::plugin::Qukey(QWERTY, KeyAddr(3, 7), MO(FUN)),
  )
  Qukeys.setHoldTimeout(1000);
  Qukeys.setOverlapThreshold(50);
  Qukeys.setMinimumHoldTime(100);
  Qukeys.setMinimumPriorInterval(80);

  Kaleidoscope.setup();
}

void loop() {
  Kaleidoscope.loop();
}
