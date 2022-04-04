/* Function Declarations of the chip-8 instructions for dy-chip8-reborn
 * For what the functions do - visit the source file.
 */

#ifndef REBORN_CHIP_INSTRUCTIONS_H
#define REBORN_CHIP_INSTRUCTIONS_H
#include "chip.h"
#include "helpers.h"
#include <string.h>

void instruction_00e0(struct chip8_sys* chip8, struct ops* ops,
                      SDL_mutex* pixels_mutex);
void instruction_00ee(struct chip8_sys* chip8, struct ops* ops);
void instruction_1nnn(struct chip8_sys* chip8, struct ops* ops);
void instruction_2nnn(struct chip8_sys* chip8, struct ops* ops);
void instruction_3xnn(struct chip8_sys* chip8, struct ops* ops);
void instruction_4xnn(struct chip8_sys* chip8, struct ops* ops);
void instruction_5xy0(struct chip8_sys* chip8, struct ops* ops);
void instruction_6xnn(struct chip8_sys* chip8, struct ops* ops);
void instruction_7xnn(struct chip8_sys* chip8, struct ops* ops);
void instruction_8xy0(struct chip8_sys* chip8, struct ops* ops);
void instruction_8xy1(struct chip8_sys* chip8, struct ops* ops);
void instruction_8xy2(struct chip8_sys* chip8, struct ops* ops);
void instruction_8xy3(struct chip8_sys* chip8, struct ops* ops);
void instruction_8xy4(struct chip8_sys* chip8, struct ops* ops);
void instruction_8xy5(struct chip8_sys* chip8, struct ops* ops);
void instruction_8xy6(struct chip8_sys* chip8, struct ops* ops);
void instruction_8xy7(struct chip8_sys* chip8, struct ops* ops);
void instruction_8xye(struct chip8_sys* chip8, struct ops* ops);
void instruction_9xy0(struct chip8_sys* chip8, struct ops* ops);
void instruction_annn(struct chip8_sys* chip8, struct ops* ops);
void instruction_bnnn(struct chip8_sys* chip8, struct ops* ops);
void instruction_cxnn(struct chip8_sys* chip8, struct ops* ops);
void instruction_dxyn(struct state* s);
void instruction_ex9e(struct state* s);
void instruction_exa1(struct state* s);
void instruction_fx07(struct chip8_sys* chip8, struct ops* ops);
void instruction_fx0a(struct state* s);
void instruction_fx15(struct chip8_sys* chip8, struct ops* ops);
void instruction_fx18(struct chip8_sys* chip8, struct ops* ops);
void instruction_fx1e(struct chip8_sys* chip8, struct ops* ops);
void instruction_fx29(struct chip8_sys* chip8, struct ops* ops);
void instruction_fx33(struct chip8_sys* chip8, struct ops* ops);
void instruction_fx55(struct chip8_sys* chip8, struct ops* ops);
void instruction_fx65(struct chip8_sys* chip8, struct ops* ops);

#endif
