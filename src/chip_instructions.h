/* Function Declarations of the chip-8 instructions for dy-chip8-reborn */

#ifndef REBORN_CHIP_INSTRUCTIONS_H
#define REBORN_CHIP_INSTRUCTIONS_H
#include "chip.h"
#include "helpers.h"
#include <string.h>

void instruction_00e0(struct state* s);
void instruction_00ee(struct state* s);
void instruction_1nnn(struct state* s);
void instruction_2nnn(struct state* s);
void instruction_3xnn(struct state* s);
void instruction_4xnn(struct state* s);
void instruction_5xy0(struct state* s);
void instruction_6xnn(struct state* s);
void instruction_7xnn(struct state* s);
void instruction_8xy0(struct state* s);

#endif