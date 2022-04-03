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
#include <SDL2/SDL_atomic.h>
#include <stdint.h>
#include <time.h>

/* clear screen */
void instruction_00e0(struct chip8_sys* chip8, struct ops* op)
{
    (void)op;
    memset(chip8->display, 0, DISPLAY_SIZE);
}

/* return from subroutine */
void instruction_00ee(struct chip8_sys* chip8, struct ops* op)
{
    (void)op;
    chip8->program_counter = pop(chip8);
}

/* jump to address */
void instruction_1nnn(struct chip8_sys* chip8, struct ops* op)
{
    chip8->program_counter = op->NNN;
}

/* call function at address */
void instruction_2nnn(struct chip8_sys* chip8, struct ops* op)
{
    push(chip8, chip8->program_counter);
    instruction_1nnn(chip8, op);
}

/* skip instruction if VX == NN */
void instruction_3xnn(struct chip8_sys* chip8, struct ops* op)
{
    if (chip8->registers[op->X] == op->NN)
        chip8->program_counter += 2;
}

/* skip instruction if VX != NN */
void instruction_4xnn(struct chip8_sys* chip8, struct ops* op)
{
    if (chip8->registers[op->X] != op->NN)
        chip8->program_counter += 2;
}

/* skip instruction if VX == XY */
void instruction_5xy0(struct chip8_sys* chip8, struct ops* op)
{
    if (chip8->registers[op->X] == chip8->registers[op->Y])
        chip8->program_counter += 2;
}

/* store NN in VX*/
void instruction_6xnn(struct chip8_sys* chip8, struct ops* op)
{
    chip8->registers[op->X] = op->NN;
}

/* add NN to VX*/
void instruction_7xnn(struct chip8_sys* chip8, struct ops* op)
{
    chip8->registers[op->X] += op->NN;
}

/* store VY in VX */
void instruction_8xy0(struct chip8_sys* chip8, struct ops* op)
{
    chip8->registers[op->X] = chip8->registers[op->Y];
}

/* set VX = VX OR VY */
void instruction_8xy1(struct chip8_sys* chip8, struct ops* op)
{
    chip8->registers[op->X] |= chip8->registers[op->Y];
}

/* set VX = VX AND VY */
void instruction_8xy2(struct chip8_sys* chip8, struct ops* op)
{
    chip8->registers[op->X] &= chip8->registers[op->Y];
}

/* set VX = VX XOR VY */
void instruction_8xy3(struct chip8_sys* chip8, struct ops* op)
{
    chip8->registers[op->X] ^= chip8->registers[op->Y];
}

/* add VY to VX and set carry flag (VF) */
void instruction_8xy4(struct chip8_sys* chip8, struct ops* op)
{
    chip8->registers[0xF] =
        (chip8->registers[op->X] > UINT16_MAX - chip8->registers[op->Y]);

    chip8->registers[op->X] += chip8->registers[op->Y];
}

/* sub VY from VX and set VF if it does not borrow */
void instruction_8xy5(struct chip8_sys* chip8, struct ops* op)
{
    /* Assume it does not borrow*/
    chip8->registers[0xF] = 1;

    /* Well if it does then just unset or 0*/
    if (chip8->registers[op->X] < chip8->registers[op->Y])
        chip8->registers[0xF] = 0;

    chip8->registers[op->X] -= chip8->registers[op->Y];
}

/* Set VX = Right Shift VY by 1 and Set VF = LSB(VY) */
void instruction_8xy6(struct chip8_sys* chip8, struct ops* op)
{
    chip8->registers[op->X] = chip8->registers[op->Y] >> 1;

    chip8->registers[0xF] = chip8->registers[op->Y] & 1;
}

/* sub VX from VY and set VF if it does not borrow */
void instruction_8xy7(struct chip8_sys* chip8, struct ops* op)
{
    /* Assume it does not borrow*/
    chip8->registers[0xF] = 1;

    /* Well if it does then just unset or 0*/
    if (chip8->registers[op->Y] < chip8->registers[op->X])
        chip8->registers[0xF] = 0;

    chip8->registers[op->X] = chip8->registers[op->Y] - chip8->registers[op->X];
}

/* Set VX = Left Shift VY by 1 and Set VF = MSB(VY) */
void instruction_8xye(struct chip8_sys* chip8, struct ops* op)
{
    chip8->registers[op->X] = chip8->registers[op->Y] << 1;

    chip8->registers[0xF] = chip8->registers[op->Y] & (1 << 7);
}

/* skip instruction if VX != XY */
void instruction_9xy0(struct chip8_sys* chip8, struct ops* op)
{
    if (chip8->registers[op->X] != chip8->registers[op->Y])
        chip8->program_counter += 2;
}

/* Store address in Register I */
void instruction_annn(struct chip8_sys* chip8, struct ops* op)
{
    chip8->index = op->NNN;
}

/* Jump to address NNN + V0 */
void instruction_bnnn(struct chip8_sys* chip8, struct ops* op)
{
    chip8->program_counter = op->NNN + chip8->registers[0];
}

/* Set VX to random number masked with NN */
void instruction_cxnn(struct chip8_sys* chip8, struct ops* op)
{
    chip8->registers[op->X] = (rand() % UINT16_MAX) & op->NN;
}

/* draw sprite at (VX,VY) with sprite data from address stored at VI*/
void instruction_dxyn(struct state* s)
{
    uint8_t x = s->chip8->registers[s->ops->X] & (DISPW - 1);
    uint8_t y = s->chip8->registers[s->ops->Y] & (DISPH - 1);

    s->chip8->registers[0xF] = 0;

    for (int h = 0; h < s->ops->N; h++) {
        /* dont draw on the bottom edge */
        if (h + y >= DISPH)
            return;

        uint16_t pixel = s->chip8->memory[s->chip8->index + h];

        for (int w = 0; w < 8; w++) {
            /* dont draw on the right edge */
            if (w + x >= DISPW)
                return;
            /* if the pixel to be rendered is not zero */
            if (pixel & (0x80 >> w)) {
                /* if the pixel already on display is one
                 * then set VF to 1 to indicate collision
                 */
                if (s->chip8->display[(x + w) + ((y + h) * DISPW)])
                    s->chip8->registers[0xF] = 1;

                /* Simply XOR with pixel since its known that
                 * pixel on display is already zero
                 */
                s->chip8->display[(x + w) + ((y + h) * DISPW)] ^= 1;
            }
        }
    }
    SDL_AtomicSet(&s->DrawFL, TRUE);
}