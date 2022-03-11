/* Implementation of the chip-8 instructions for dy-chip8-reborn
 * Author: Amritpal Singh (C) 2022
 * For a list of instructions and what they do please visit any of the following
 * links:
 * https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Instruction-Set
 * http://johnearnest.github.io/Octo/docs/chip8ref.pdf
 *
 */

#include "chip_instructions.h"
#include "chip.h"
#include "helpers.h"

/* clear screen */
void instruction_00e0(struct state* s)
{
    memset(s->chip8->display, 0, DISPLAY_SIZE);
}

/* return from subroutine */
void instruction_00ee(struct state* s)
{
    s->chip8->program_counter = pop(s);
}

/* jump to address */
void instruction_1nnn(struct state* s)
{
    s->chip8->program_counter = s->ops->NNN;
}

/* call function at address */
void instruction_2nnn(struct state* s)
{
    push(s, s->chip8->program_counter);
    instruction_1nnn(s);
}

/* skip instruction if VX == NN */
void instruction_3xnn(struct state* s)
{
    if (s->chip8->registers[s->ops->X] == s->ops->NN)
        s->chip8->program_counter += 2;
}

/* skip instruction if VX != NN */
void instruction_4xnn(struct state* s)
{
    if (s->chip8->registers[s->ops->X] != s->ops->NN)
        s->chip8->program_counter += 2;
}

/* skip instruction if VX == XY */
void instruction_5xy0(struct state* s)
{
    if (s->chip8->registers[s->ops->X] == s->chip8->registers[s->ops->Y])
        s->chip8->program_counter += 2;
}

/* store NN in VX*/
void instruction_6xnn(struct state* s)
{
    s->chip8->registers[s->ops->X] = s->ops->NN;
}

/* add NN to VX*/
void instruction_7xnn(struct state* s)
{
    s->chip8->registers[s->ops->X] += s->ops->NN;
}

/* store VY in VX */
void instruction_8xy0(struct state* s)
{
    s->chip8->registers[s->ops->X] = s->chip8->registers[s->ops->Y];
}