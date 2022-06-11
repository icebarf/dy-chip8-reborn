#include "keyboard.h"
#include <assert.h>

void check_and_modify_keystate(const Uint8* SDL_Keyboard_State, struct state* const emulator_state)
{
    assert(emulator_state);
    assert(SDL_Keyboard_State);

    /* 1    2    3    4*/
    if (SDL_Keyboard_State[SDL_SCANCODE_1])
        emulator_state->keystates[0x1] = UP;
    else
        emulator_state->keystates[0x1] = DOWN;

    if (SDL_Keyboard_State[SDL_SCANCODE_2])
        emulator_state->keystates[0x2] = UP;
    else
        emulator_state->keystates[0x2] = DOWN;

    if (SDL_Keyboard_State[SDL_SCANCODE_3])
        emulator_state->keystates[0x3] = UP;
    else
        emulator_state->keystates[0x3] = DOWN;

    if (SDL_Keyboard_State[SDL_SCANCODE_4])
        emulator_state->keystates[0xC] = UP;
    else
        emulator_state->keystates[0xC] = DOWN;

    /* Q    W    E    R */
    if (SDL_Keyboard_State[SDL_SCANCODE_Q])
        emulator_state->keystates[0x4] = UP;
    else
        emulator_state->keystates[0x4] = DOWN;

    if (SDL_Keyboard_State[SDL_SCANCODE_W])
        emulator_state->keystates[0x5] = UP;
    else
        emulator_state->keystates[0x5] = DOWN;

    if (SDL_Keyboard_State[SDL_SCANCODE_E])
        emulator_state->keystates[0x6] = UP;
    else
        emulator_state->keystates[0x6] = DOWN;

    if (SDL_Keyboard_State[SDL_SCANCODE_R])
        emulator_state->keystates[0xD] = UP;
    else
        emulator_state->keystates[0xD] = DOWN;

    /* A    S    D    F */
    if (SDL_Keyboard_State[SDL_SCANCODE_A])
        emulator_state->keystates[0x7] = UP;
    else
        emulator_state->keystates[0x7] = DOWN;

    if (SDL_Keyboard_State[SDL_SCANCODE_S])
        emulator_state->keystates[0x8] = UP;
    else
        emulator_state->keystates[0x8] = DOWN;

    if (SDL_Keyboard_State[SDL_SCANCODE_D])
        emulator_state->keystates[0x9] = UP;
    else
        emulator_state->keystates[0x9] = DOWN;

    if (SDL_Keyboard_State[SDL_SCANCODE_F])
        emulator_state->keystates[0xE] = UP;
    else
        emulator_state->keystates[0xE] = DOWN;

    /* Z    X    C    V */
    if (SDL_Keyboard_State[SDL_SCANCODE_Z])
        emulator_state->keystates[0xA] = UP;
    else
        emulator_state->keystates[0xA] = DOWN;

    if (SDL_Keyboard_State[SDL_SCANCODE_X])
        emulator_state->keystates[0x0] = UP;
    else
        emulator_state->keystates[0x0] = DOWN;

    if (SDL_Keyboard_State[SDL_SCANCODE_C])
        emulator_state->keystates[0xB] = UP;
    else
        emulator_state->keystates[0xB] = DOWN;

    if (SDL_Keyboard_State[SDL_SCANCODE_V])
        emulator_state->keystates[0xF] = UP;
    else
        emulator_state->keystates[0xF] = DOWN;
}
