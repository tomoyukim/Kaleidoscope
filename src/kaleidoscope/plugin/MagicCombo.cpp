/* -*- mode: c++ -*-
 * Kaleidoscope-MagicCombo -- Magic combo framework
 * Copyright (C) 2016, 2017, 2018, 2020  Keyboard.io, Inc
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

#include <Kaleidoscope-MagicCombo.h>
#include "kaleidoscope/keyswitch_state.h"

namespace kaleidoscope {
namespace plugin {

uint16_t MagicCombo::min_interval = 500;
uint16_t MagicCombo::start_time_ = 0;
uint8_t MagicCombo::longest_partial_match_;
uint16_t MagicCombo::partial_match_start_time_;
uint16_t MagicCombo::partial_match_timeout_ = 60 * 5;

bool MagicCombo::checkPartialCombos() {
  for (byte i = 0; i < magiccombo::combos_length; i++) {
    bool match = true;
    byte j;

    for (j = 0; j < longest_partial_match_ + 1; j++) {
      int8_t comboKey = pgm_read_byte(&(magiccombo::combos[i].keys[j]));

      if (comboKey == 0)
        break;

      match &= Runtime.device().isKeyswitchPressed(comboKey);
      if (!match)
        break;
    }

    // If we have at least a partial match, return immediately.
    if (match)
      return true;
  }

  return false;
}

void MagicCombo::replayPartialCombos() {
  for (byte i = 0; i < magiccombo::combos_length; i++) {
    bool match = true;
    byte j;

    for (j = 0; j < longest_partial_match_ + 1; j++) {
      int8_t comboKey = pgm_read_byte(&(magiccombo::combos[i].keys[j]));

      if (comboKey == 0)
        break;

      match &= Runtime.device().isKeyswitchPressed(comboKey);
      if (!match)
        break;
    }

    // If we have at least a partial match, replay it until the current match
    // index.
    if (match) {
      for (byte k = 0; k < j; k++) {
        uint8_t comboKey = pgm_read_byte(&(magiccombo::combos[i].keys[k]));
        handleKeyswitchEvent(Key_NoKey, KeyAddr(comboKey),
                             IS_PRESSED | WAS_PRESSED | INJECTED);
      }
    }
  }
}

void MagicCombo::updateComboState() {
  for (byte i = 0; i < magiccombo::combos_length; i++) {
    bool match = true;
    byte j;

    for (j = 0; j < MAX_COMBO_LENGTH; j++) {
      int8_t comboKey = pgm_read_byte(&(magiccombo::combos[i].keys[j]));

      if (comboKey == 0)
        break;

      match &= Runtime.device().isKeyswitchPressed(comboKey);
      if (!match)
        break;

      if (j > longest_partial_match_) {
        longest_partial_match_ = j;
      }
    }
  }
}

bool MagicCombo::isPartialComplete() {
  for (byte i = 0; i < magiccombo::combos_length; i++) {
    bool match = true;
    byte j;

    for (j = 0; j < MAX_COMBO_LENGTH; j++) {
      int8_t comboKey = pgm_read_byte(&(magiccombo::combos[i].keys[j]));

      if (comboKey == 0)
        break;

      match &= Runtime.device().isKeyswitchPressed(comboKey);
      if (!match)
        break;
    }

    if (j != Runtime.device().pressedKeyswitchCount())
      match = false;

    if (match)
      return true;
  }

  return false;
}

void MagicCombo::playComboActions() {
  for (byte i = 0; i < magiccombo::combos_length; i++) {
    bool match = true;
    byte j;

    for (j = 0; j < MAX_COMBO_LENGTH; j++) {
      int8_t comboKey = pgm_read_byte(&(magiccombo::combos[i].keys[j]));

      if (comboKey == 0)
        break;

      match &= Runtime.device().isKeyswitchPressed(comboKey);
      if (!match)
        break;
    }

    if (j != Runtime.device().pressedKeyswitchCount())
      match = false;

    if (!match)
      break;

    ComboAction action = (ComboAction) pgm_read_ptr((void const **) & (magiccombo::combos[i].action));

    (*action)(i);
  }
}

EventHandlerResult MagicCombo::onKeyswitchEvent(Key &mapped_key,
                                                KeyAddr key_addr, uint8_t key_state) {
  // If handling an INJECTED event, pass through. We should be the first plugin
  // in the list, and we only care about explicit physical key presses, not
  // injected events.
  if (key_state & INJECTED)
    return EventHandlerResult::OK;

  // If the current state does not have a partial...
  if (!checkPartialCombos()) {
    // ..then replay any previous partials, and let this event pass through.

    replayPartialCombos();
    updateComboState();
    return EventHandlerResult::OK;
  }

  // If we do have a partial, then if this current event is a new key toggling
  // on, restart our partial expiry timer.
  if (keyToggledOn(key_state)) {
    partial_match_start_time_ = Runtime.millisAtCycleStart();
  }

  // If we have a non-complete match...
  if (!isPartialComplete()) {
    // ...and the timer expired, replay the match.
    if (Runtime.hasTimeExpired(partial_match_start_time_,
                                partial_match_timeout_)) {
      updateComboState();
      replayPartialCombos();

      // We consume the event, because we've replayed it above.
      return EventHandlerResult::EVENT_CONSUMED;
    } else { // Otherwise just consume the event
      updateComboState();

      return EventHandlerResult::EVENT_CONSUMED;
    }
  }

  // We now have a complete match.

  // If the repeat timer expired, play the action for matching combos.
  if (Runtime.hasTimeExpired(start_time_, min_interval)) {
    playComboActions();
    start_time_ = Runtime.millisAtCycleStart();
  }

  return EventHandlerResult::EVENT_CONSUMED;
}

#if 0
EventHandlerResult MagicCombo::beforeReportingState() {
  for (byte i = 0; i < magiccombo::combos_length; i++) {
    bool match = true;
    byte j;

    for (j = 0; j < MAX_COMBO_LENGTH; j++) {
      int8_t comboKey = pgm_read_byte(&(magiccombo::combos[i].keys[j]));

      if (comboKey == 0)
        break;

      match &= Runtime.device().isKeyswitchPressed(comboKey);
      if (!match)
        break;
    }

    if (j != Runtime.device().pressedKeyswitchCount())
      match = false;

    if (match && Runtime.hasTimeExpired(start_time_, min_interval)) {
      ComboAction action = (ComboAction) pgm_read_ptr((void const **) & (magiccombo::combos[i].action));

      (*action)(i);
      start_time_ = Runtime.millisAtCycleStart();
    }
  }

  return EventHandlerResult::OK;
}
#endif

}
}

kaleidoscope::plugin::MagicCombo MagicCombo;
