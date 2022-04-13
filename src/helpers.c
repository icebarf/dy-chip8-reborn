#include "chip.h"
#include <stdio.h>

uint16_t pop(struct chip8_sys* chip8)
{
    return chip8->stack[chip8->stacktop--];
}

void push(struct chip8_sys* chip8, const uint16_t x)
{
    chip8->stack[++chip8->stacktop] = x;
}