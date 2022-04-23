#include "keyboard.h"
#include <assert.h>

void check_and_modify_keystate(struct state* const emulator_state)
{
    assert(emulator_state);

    /* 1    2    3    4*/
    if (IsKeyPressed(30))
        emulator_state->keystates[0x1] = UP;
    else
        emulator_state->keystates[0x1] = DOWN;

    if (IsKeyPressed(31))
        emulator_state->keystates[0x2] = UP;
    else
        emulator_state->keystates[0x2] = DOWN;

    if (IsKeyPressed(32))
        emulator_state->keystates[0x3] = UP;
    else
        emulator_state->keystates[0x3] = DOWN;

    if (IsKeyPressed(33))
        emulator_state->keystates[0xC] = UP;
    else
        emulator_state->keystates[0xC] = DOWN;

    /* Q    W    E    R */
    if (IsKeyPressed(20))
        emulator_state->keystates[0x4] = UP;
    else
        emulator_state->keystates[0x4] = DOWN;

    if (IsKeyPressed(26))
        emulator_state->keystates[0x5] = UP;
    else
        emulator_state->keystates[0x5] = DOWN;

    if (IsKeyPressed(8))
        emulator_state->keystates[0x6] = UP;
    else
        emulator_state->keystates[0x6] = DOWN;

    if (IsKeyPressed(21))
        emulator_state->keystates[0xD] = UP;
    else
        emulator_state->keystates[0xD] = DOWN;

    /* A    S    D    F */
    if (IsKeyPressed(4))
        emulator_state->keystates[0x7] = UP;
    else
        emulator_state->keystates[0x7] = DOWN;

    if (IsKeyPressed(22))
        emulator_state->keystates[0x8] = UP;
    else
        emulator_state->keystates[0x8] = DOWN;

    if (IsKeyPressed(7))
        emulator_state->keystates[0x9] = UP;
    else
        emulator_state->keystates[0x9] = DOWN;

    if (IsKeyPressed(9))
        emulator_state->keystates[0xE] = UP;
    else
        emulator_state->keystates[0xE] = DOWN;

    /* Z    X    C    V */
    if (IsKeyPressed(29))
        emulator_state->keystates[0xA] = UP;
    else
        emulator_state->keystates[0xA] = DOWN;

    if (IsKeyPressed(27))
        emulator_state->keystates[0x0] = UP;
    else
        emulator_state->keystates[0x0] = DOWN;

    if (IsKeyPressed(6))
        emulator_state->keystates[0xB] = UP;
    else
        emulator_state->keystates[0xB] = DOWN;

    if (IsKeyPressed(25))
        emulator_state->keystates[0xF] = UP;
    else
        emulator_state->keystates[0xF] = DOWN;
}