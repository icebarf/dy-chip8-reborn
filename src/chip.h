#ifndef REBORN_CHIP_H
#define REBORN_CHIP_H

#include <SDL2/SDL.h>

typedef uint8_t Bool;

#define TIMER_DEC_RATE (double)16666666.666667

// clang-format off
enum CONSTANTS {
    /* truths */
    FALSE = 0,
    TRUE = 1,
    DOWN = 0,
    UP = 1,

    /* emulator specific */
    INST_CNT = 35,
    DISPW = 64,
    DISPH = 32,
    MEMSIZE = 4096,
    REGNUM = 16,
    STACKSIZE = 48,
    DISPLAY_SIZE = DISPW * DISPH,
    KEYS = 16,
    PROGRAM_LOAD_ADDRESS = 0x200,
    INITIAL_STACK_TOP_LOCATION = -1,

    /* errors */
    BAD_RETURN_VALUE = -1,
    
    /* extras */
    BASE_10 = 10,
    BASE_16 = 16
};
// clang-format on

struct chip8_sys {
    uint8_t memory[MEMSIZE];
    uint8_t display[DISPLAY_SIZE];
    uint16_t stack[STACKSIZE];
    uint8_t registers[REGNUM];
    uint16_t index;
    uint16_t program_counter;
    uint8_t delay_timer;
    uint8_t sound_timer;
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
    uint32_t color;
};

struct state {
    uint8_t keystates[KEYS];
    struct chip8_sys* chip8;
    struct ops* ops;
    struct sdl_objs* sdl_objs;
    struct chip8_launch_data* data;
    double current_counter_val;
    double previous_counter_val;
    double delta_time;
    double delta_accumulation;
    uint8_t run;
    uint8_t DrawFL;
};

struct chip8_launch_data {
    char* rom_path;
    unsigned long frequency;
    uint32_t bg;
    uint32_t fg;
    Bool quirks;
    Bool yes_rom;
    Bool debugger;
};

/* define some popular escape sequences */
/* visit https://github.com/dylanaraps/pure-bash-bible#text-colors for more
 * info*/
// clang-format off
#define RESET       "\033[m"

#define BOLD        "\033[1m"
#define FAINT       "\033[2m"
#define ITALIC      "\033[3m"
#define ULINE       "\033[4m"
#define BLINK       "\033[5m"
#define HIGHLIGHT   "\033[7m"

#define GREEN       "\033[38;2;0;255;0m"
#define GREEN_2     "\033[32m"
#define RED         "\033[38;2;255;0;0m"
#define RED_2       "\033[31m"
#define BLUE        "\033[34m"
// clang-format on

#endif
