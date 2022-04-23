#include "keyboard.h"
#include <assert.h>

#ifndef RAYLIB_H
#include <raylib.h>
#endif


void check_and_modify_keystate(struct state* const emulator_state)
{
    assert(emulator_state);

    /* 1    2    3    4*/
    if (IsKeyDown(KEY_ONE))
        emulator_state->keystates[0x1] = UP;
    else
        emulator_state->keystates[0x1] = DOWN;

    if (IsKeyDown(KEY_TWO))
        emulator_state->keystates[0x2] = UP;
    else
        emulator_state->keystates[0x2] = DOWN;

    if (IsKeyDown(KEY_THREE))
        emulator_state->keystates[0x3] = UP;
    else
        emulator_state->keystates[0x3] = DOWN;

    if (IsKeyDown(KEY_FOUR))
        emulator_state->keystates[0xC] = UP;
    else
        emulator_state->keystates[0xC] = DOWN;

    /* Q    W    E    R */
    if (IsKeyDown(KEY_Q))
        emulator_state->keystates[0x4] = UP;
    else
        emulator_state->keystates[0x4] = DOWN;

    if (IsKeyDown(KEY_W))
        emulator_state->keystates[0x5] = UP;
    else
        emulator_state->keystates[0x5] = DOWN;

    if (IsKeyDown(KEY_E))
        emulator_state->keystates[0x6] = UP;
    else
        emulator_state->keystates[0x6] = DOWN;

    if (IsKeyDown(KEY_T))
        emulator_state->keystates[0xD] = UP;
    else
        emulator_state->keystates[0xD] = DOWN;

    /* A    S    D    F */
    if (IsKeyDown(KEY_A))
        emulator_state->keystates[0x7] = UP;
    else
        emulator_state->keystates[0x7] = DOWN;

    if (IsKeyDown(KEY_S))
        emulator_state->keystates[0x8] = UP;
    else
        emulator_state->keystates[0x8] = DOWN;

    if (IsKeyDown(KEY_D))
        emulator_state->keystates[0x9] = UP;
    else
        emulator_state->keystates[0x9] = DOWN;

    if (IsKeyDown(KEY_F))
        emulator_state->keystates[0xE] = UP;
    else
        emulator_state->keystates[0xE] = DOWN;

    /* Z    X    C    V */
    if (IsKeyDown(KEY_Z))
        emulator_state->keystates[0xA] = UP;
    else
        emulator_state->keystates[0xA] = DOWN;

    if (IsKeyDown(KEY_X))
        emulator_state->keystates[0x0] = UP;
    else
        emulator_state->keystates[0x0] = DOWN;

    if (IsKeyDown(KEY_C))
        emulator_state->keystates[0xB] = UP;
    else
        emulator_state->keystates[0xB] = DOWN;

    if (IsKeyDown(KEY_V))
        emulator_state->keystates[0xF] = UP;
    else
        emulator_state->keystates[0xF] = DOWN;
}