#ifndef REBORN_CHIP_H
#define REBORN_CHIP_H

#include <SDL2/SDL.h>
#include <stdint.h>

enum MUTEX { MEMORY };
enum RUN { FALSE, TRUE };
enum CHIP8 { INST_CNT = 35 };

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
 * N        - the fourth nibble
 */

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

struct state {
    struct chip8_sys* chip8;
    struct ops* ops;
    SDL_mutex** mutexes;
    SDL_atomic_t run;
};

#endif