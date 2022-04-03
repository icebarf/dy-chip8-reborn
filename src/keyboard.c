#include "keyboard.h"
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_stdinc.h>

void check_and_modify_keystate(const Uint8* state, struct state* s)
{
    /* 1    2    3    4*/
    if (state[SDL_SCANCODE_1])
        s->keystates[0x1] = TRUE;
    else
        s->keystates[0x1] = FALSE;

    if (state[SDL_SCANCODE_2])
        s->keystates[0x2] = TRUE;
    else
        s->keystates[0x2] = FALSE;

    if (state[SDL_SCANCODE_3])
        s->keystates[0x3] = TRUE;
    else
        s->keystates[0x3] = FALSE;

    if (state[SDL_SCANCODE_4])
        s->keystates[0xC] = TRUE;
    else
        s->keystates[0xC] = FALSE;

    /* Q    W    E    R */
    if (state[SDL_SCANCODE_Q])
        s->keystates[0x4] = TRUE;
    else
        s->keystates[0x4] = FALSE;

    if (state[SDL_SCANCODE_W])
        s->keystates[0x5] = TRUE;
    else
        s->keystates[0x5] = FALSE;

    if (state[SDL_SCANCODE_E])
        s->keystates[0x6] = TRUE;
    else
        s->keystates[0x6] = FALSE;

    if (state[SDL_SCANCODE_R])
        s->keystates[0xD] = TRUE;
    else
        s->keystates[0xD] = FALSE;

    /* A    S    D    F */
    if (state[SDL_SCANCODE_A])
        s->keystates[0x7] = TRUE;
    else
        s->keystates[0x7] = FALSE;

    if (state[SDL_SCANCODE_S])
        s->keystates[0x8] = TRUE;
    else
        s->keystates[0x8] = FALSE;

    if (state[SDL_SCANCODE_D])
        s->keystates[0x9] = TRUE;
    else
        s->keystates[0x9] = FALSE;

    if (state[SDL_SCANCODE_F])
        s->keystates[0xE] = TRUE;
    else
        s->keystates[0xE] = FALSE;

    /* Z    X    C    V */
    if (state[SDL_SCANCODE_Z])
        s->keystates[0xA] = TRUE;
    else
        s->keystates[0xA] = FALSE;

    if (state[SDL_SCANCODE_X])
        s->keystates[0x0] = TRUE;
    else
        s->keystates[0x0] = FALSE;

    if (state[SDL_SCANCODE_C])
        s->keystates[0xB] = TRUE;
    else
        s->keystates[0xB] = FALSE;

    if (state[SDL_SCANCODE_V])
        s->keystates[0xF] = TRUE;
    else
        s->keystates[0xF] = FALSE;
}