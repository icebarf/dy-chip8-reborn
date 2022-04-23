#ifndef REBORN_KEYBOARD_HANDLER_H
#define REBORN_KEYBOARD_HANDLER_H

#include "chip.h"

/**
 * Parameters :
 * state of keyboard as an array of Uint8 pointer,
 * current emulator structure in the 'state' structure
 **
 * Modifies the keystates array in emulator state
 * sets keystates in emulator structure by looking at the keyboard state received
 **/
void check_and_modify_keystate(struct state* const emulator_state);

#endif