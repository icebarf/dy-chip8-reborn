#ifndef REBORN_HELPERS_H
#define REBORN_HELPERS_H
#include <stdint.h>

void print_program_memory(struct chip8_sys* chip8);

uint16_t pop(struct chip8_sys* chip8);
void push(struct chip8_sys* chip8, uint16_t x);

#endif
