#include "chip.h"
#include <SDL2/SDL_timer.h>
#include <stdio.h>

uint16_t pop(struct chip8_sys* chip8)
{
    return chip8->stack[chip8->stacktop--];
}

void push(struct chip8_sys* chip8, const uint16_t x)
{
    chip8->stack[++chip8->stacktop] = x;
}

double get_delta_time(const double current_counter_val, const double prev_counter_val)
{
    return ((current_counter_val - prev_counter_val) * 1000000000.0) /
           SDL_GetPerformanceFrequency();
}