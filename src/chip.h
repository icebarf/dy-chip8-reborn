#ifndef REBORN_CHIP_H
#define REBORN_CHIP_H

#include <SDL2/SDL.h>
#include <stdint.h>

enum constants {
    FALSE = 0,
    TRUE = 1,
    DOWN = 0,
    UP = 1,
    INST_CNT = 35,
    DISPW = 64,
    DISPH = 32,
    MEMSIZE = 4096,
    REGNUM = 16,
    STACKSIZE = 48,
    DISPLAY_SIZE = DISPW * DISPH,
    KEYS = 16
};

struct chip8_sys {
    uint8_t memory[MEMSIZE];
    uint16_t stack[STACKSIZE];
    uint8_t registers[REGNUM];
    uint8_t display[DISPLAY_SIZE];

    uint16_t index;
    uint16_t program_counter;
    SDL_atomic_t delay_timer;
    SDL_atomic_t sound_timer;

    uint8_t stacktop;
};

/* different values related to instructions -
 * https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Instruction-Set
 *
 * opcode   - full 16-bit instruction
 * NNN      - second, third, fourth nibble - a 12bit number in a 16-bit value
 * NN       - lower byte
 * inst     - upper byte
 * inst_nib - the first  nibble
 * X        - the second nibble
 * Y        - the third  nibble
 * N        - the fourth nibble */

struct ops {
    uint16_t opcode;
    uint16_t NNN;
    uint8_t NN;
    uint8_t inst;
    uint8_t inst_nib;
    uint8_t X;
    uint8_t Y;
    uint8_t N;
};

struct sdl_objs {
    SDL_Window* screen;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    uint32_t* pixels;
};

struct atoms {
    SDL_atomic_t* run;
    SDL_atomic_t* timer;
};

struct state {
    struct chip8_sys* chip8;
    struct ops* ops;
    struct sdl_objs* sdl_objs;
    SDL_mutex* pixels_mutex;
    SDL_mutex* main_mutex;
    SDL_atomic_t run;
    uint8_t keystates[KEYS];
    SDL_atomic_t DrawFL;
};

#endif
