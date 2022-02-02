#ifndef REBORN_CHIP_H
#define REBORN_CHIP_H

#include <SDL2/SDL.h>

enum RUN { FALSE, TRUE };

/* The chip8 system */
struct chip8_sys {

    const enum constants {
        MEMSIZE = 4096,
        REGNUM = 16,
        STACKSIZE = 48,
        DISPLAY_SIZE = 64 * 32
    } constants;

    uint8_t memory[MEMSIZE];
    uint16_t stack[STACKSIZE];
    uint8_t registers[REGNUM];
    uint8_t display[DISPLAY_SIZE];

    uint16_t index;
    uint16_t program_counter;
    SDL_atomic_t delay_timer;
    SDL_atomic_t sound_timer;

    int stacktop;
};

#endif