#ifndef REBORN_KEYBOARD_HANDLER_H
#define REBORN_KEYBOARD_HANDLER_H

#include "chip.h"
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>

void check_and_modify_keystate(const Uint8* state, struct state* s);

#endif