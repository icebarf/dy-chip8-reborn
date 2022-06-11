#ifndef REBORN_CHIP_INSTRUCTIONS_IMPL
#define REBORN_CHIP_INSTRUCTIONS_IMPL
/* Implementation of the chip-8 instructions for dy-chip8-reborn
 * Author: Amritpal Singh (C) 2022
 * For a list of instructions and what they do please visit any of the following
 * links:
 * https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Instruction-Set
 * http://johnearnest.github.io/Octo/docs/chip8ref.pdf
 *
 */

#include "chip.h"
#include "helpers.h"
#include <stdint.h>
#include <time.h>

/* clear screen */
[[gnu::always_inline]] static inline void instruction_00e0(struct state* s)
{
    memset(s->chip8->display, 0, DISPLAY_SIZE);
    s->DrawFL = TRUE;
}

/* return from subroutine */
[[gnu::always_inline]] static inline void instruction_00ee(struct chip8_sys* chip8)
{
    chip8->program_counter = pop(chip8);
}

/* jump to address */
[[gnu::always_inline]] static inline void instruction_1nnn(struct chip8_sys* chip8, struct ops* op)
{
    chip8->program_counter = op->NNN;
}

/* call function at address */
[[gnu::always_inline]] static inline void instruction_2nnn(struct chip8_sys* chip8, struct ops* op)
{
    push(chip8, chip8->program_counter);
    instruction_1nnn(chip8, op);
}

/* skip instruction if VX == NN */
[[gnu::always_inline]] static inline void instruction_3xnn(struct chip8_sys* chip8, struct ops* op)
{
    if (chip8->registers[op->X] == op->NN)
        chip8->program_counter += 2;
}

/* skip instruction if VX != NN */
[[gnu::always_inline]] static inline void instruction_4xnn(struct chip8_sys* chip8, struct ops* op)
{
    if (chip8->registers[op->X] != op->NN)
        chip8->program_counter += 2;
}

/* skip instruction if VX == XY */
[[gnu::always_inline]] static inline void instruction_5xy0(struct chip8_sys* chip8, struct ops* op)
{
    if (chip8->registers[op->X] == chip8->registers[op->Y])
        chip8->program_counter += 2;
}

/* store NN in VX*/
[[gnu::always_inline]] static inline void instruction_6xnn(struct chip8_sys* chip8, struct ops* op)
{
    chip8->registers[op->X] = op->NN;
}

/* add NN to VX*/
[[gnu::always_inline]] static inline void instruction_7xnn(struct chip8_sys* chip8, struct ops* op)
{
    chip8->registers[op->X] += op->NN;
}

/* store VY in VX */
[[gnu::always_inline]] static inline void instruction_8xy0(struct chip8_sys* chip8, struct ops* op)
{
    chip8->registers[op->X] = chip8->registers[op->Y];
}

/* set VX = VX OR VY */
[[gnu::always_inline]] static inline void instruction_8xy1(struct chip8_sys* chip8, struct ops* op)
{
    chip8->registers[op->X] |= chip8->registers[op->Y];
}

/* set VX = VX AND VY */
[[gnu::always_inline]] static inline void instruction_8xy2(struct chip8_sys* chip8, struct ops* op)
{
    chip8->registers[op->X] &= chip8->registers[op->Y];
}

/* set VX = VX XOR VY */
[[gnu::always_inline]] static inline void instruction_8xy3(struct chip8_sys* chip8, struct ops* op)
{
    chip8->registers[op->X] ^= chip8->registers[op->Y];
}

/* add VY to VX and set carry flag (VF) */
[[gnu::always_inline]] static inline void instruction_8xy4(struct chip8_sys* chip8, struct ops* op)
{
    chip8->registers[0xF] = (chip8->registers[op->X] > UINT8_MAX - chip8->registers[op->Y]);

    chip8->registers[op->X] += chip8->registers[op->Y];
}

/* sub VY from VX and set VF if it does not borrow */
[[gnu::always_inline]] static inline void instruction_8xy5(struct chip8_sys* chip8, struct ops* op)
{
    chip8->registers[0xF] = 1;

    if (chip8->registers[op->X] < chip8->registers[op->Y])
        chip8->registers[0xF] = 0;

    chip8->registers[op->X] -= chip8->registers[op->Y];
}

/* Set Vx = Vx SHR 1. Set VF to LSB*/
[[gnu::always_inline]] static inline void instruction_8xy6(struct chip8_sys* chip8, struct ops* op,
                                                           struct chip8_launch_data* data)
{
    chip8->registers[0xF] = 0;
    uint8_t reg = chip8->registers[op->X];

    // implementation quirk
    if (data->quirks) {
        reg = chip8->registers[op->Y];
    }

    if (reg & 0x1)
        chip8->registers[0xF] = 1;

    chip8->registers[op->X] = reg >> 1;
}

/* sub VX from VY and set VF if it does not borrow */
[[gnu::always_inline]] static inline void instruction_8xy7(struct chip8_sys* chip8, struct ops* op)
{
    /* Assume it does not borrow*/
    chip8->registers[0xF] = 1;

    /* Well if it does then just unset or 0*/
    if (chip8->registers[op->Y] < chip8->registers[op->X])
        chip8->registers[0xF] = 0;

    chip8->registers[op->X] = chip8->registers[op->Y] - chip8->registers[op->X];
}

/* Set Vx = Vx SHL 1. Set VF to MSB*/
[[gnu::always_inline]] static inline void instruction_8xye(struct chip8_sys* chip8, struct ops* op,
                                                           struct chip8_launch_data* data)
{
    chip8->registers[0xF] = 0;
    uint8_t reg = chip8->registers[op->X];

    // implementation quirk
    if (data->quirks) {
        reg = chip8->registers[op->Y];
    }

    if (reg & 0x80)
        chip8->registers[0xF] = 1;

    chip8->registers[op->X] = reg << 1;
}

/* skip instruction if VX != XY */
[[gnu::always_inline]] static inline void instruction_9xy0(struct chip8_sys* chip8, struct ops* op)
{
    if (chip8->registers[op->X] != chip8->registers[op->Y])
        chip8->program_counter += 2;
}

/* Store address in Register I */
[[gnu::always_inline]] static inline void instruction_annn(struct chip8_sys* chip8, struct ops* op)
{
    chip8->index = op->NNN;
}

/* Jump to address NNN + V0 */
[[gnu::always_inline]] static inline void instruction_bnnn(struct chip8_sys* chip8, struct ops* op)
{
    chip8->program_counter = op->NNN + chip8->registers[0];
}

/* Set VX to random number masked with NN */
[[gnu::always_inline]] static inline void instruction_cxnn(struct chip8_sys* chip8, struct ops* op)
{
    // debug
    uint8_t rnd = rand() % UINT8_MAX;
    chip8->registers[op->X] = (rnd)&op->NN;
}

/* draw sprite at (VX,VY) with sprite data from address stored at VI*/
[[gnu::always_inline]] static inline void instruction_dxyn(struct state* s)
{
    uint8_t x = s->chip8->registers[s->ops->X] & (DISPW - 1);
    uint8_t y = s->chip8->registers[s->ops->Y] & (DISPH - 1);

    s->chip8->registers[0xF] = 0;

    for (int h = 0; h < s->ops->N; h++) {
        /* dont draw on the bottom edge */
        if (h + y >= DISPH)
            continue;

        uint16_t pixel = s->chip8->memory[s->chip8->index + h];

        for (int w = 0; w < 8; w++) {

            /* dont draw on the right edge */
            if (w + x >= DISPW)
                continue;

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

    s->DrawFL = TRUE;
}

/* skip next instruction if key in VX is UP*/
[[gnu::always_inline]] static inline void instruction_ex9e(struct state* s)
{
    if (s->keystates[s->chip8->registers[s->ops->X]] == UP)
        s->chip8->program_counter += 2;
}

/* skip next instruction if key in VX is DOWN*/
[[gnu::always_inline]] static inline void instruction_exa1(struct state* s)
{
    if (s->keystates[s->chip8->registers[s->ops->X]] == DOWN)
        s->chip8->program_counter += 2;
}

/* Store the current value of delay timer in VX */
[[gnu::always_inline]] static inline void instruction_fx07(struct chip8_sys* chip8, struct ops* ops)
{
    chip8->registers[ops->X] = chip8->delay_timer;
}

/* wait for a keypress, when pressed store the result in VX */
[[gnu::always_inline]] static inline void instruction_fx0a(struct state* s)
{
    s->chip8->program_counter -= 2;

    for (uint8_t i = 0x0; i < 0x10; i++) {

        if (s->keystates[i] == UP) {

            s->chip8->registers[s->ops->X] = i;
            s->chip8->program_counter += 2;
        }
    }
}

/* set delay timer to VX */
[[gnu::always_inline]] static inline void instruction_fx15(struct chip8_sys* chip8, struct ops* ops)
{
    chip8->delay_timer = chip8->registers[ops->X];
}

/* set sound timer to VX */
[[gnu::always_inline]] static inline void instruction_fx18(struct chip8_sys* chip8, struct ops* ops)
{
    chip8->sound_timer = chip8->registers[ops->X];
}

/* add VX to index_register */
[[gnu::always_inline]] static inline void instruction_fx1e(struct chip8_sys* chip8, struct ops* ops)
{
    chip8->index += chip8->registers[ops->X];
}

/* set index to the memory location of the sprite, which is a hex digit stored
 * in VX */
[[gnu::always_inline]] static inline void instruction_fx29(struct chip8_sys* chip8, struct ops* ops)
{
    chip8->index = 5 * (chip8->registers[ops->X] & 15);
}

/* store VX in BCD format at memory i, i+1, i+2 respectively for H,T,O
 * BCD - Binary Coded Decimal
 * HTO - Hundreds Tens Ones */
[[gnu::always_inline]] static inline void instruction_fx33(struct chip8_sys* chip8, struct ops* ops)
{
    uint8_t number = chip8->registers[ops->X];

    uint8_t o = number % 10;
    number /= 10;

    uint8_t t = number % 10;
    number /= 10;

    chip8->memory[chip8->index] = number;
    chip8->memory[chip8->index + 1] = t;
    chip8->memory[chip8->index + 2] = o;
}

/* store the value from range V0 - VX inclusive to address stored in index reg
 */
[[gnu::always_inline]] static inline void instruction_fx55(struct chip8_sys* chip8, struct ops* ops,
                                                           struct chip8_launch_data* data)
{
    memcpy(&chip8->memory[chip8->index], chip8->registers, ops->X + 1);

    // implementation quirk
    if (data->quirks)
        chip8->index += (ops->X + 1);
}

/* store values from memory address in index reg to range V0 - VX */
[[gnu::always_inline]] static inline void instruction_fx65(struct chip8_sys* chip8, struct ops* ops,
                                                           struct chip8_launch_data* data)
{
    memcpy(&chip8->registers[0], &chip8->memory[chip8->index], ops->X + 1);

    // implementation quirk
    if (data->quirks)
        chip8->index += (ops->X + 1);
}

#endif
