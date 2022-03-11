#ifndef REBORN_HELPERS_H
#define REBORN_HELPERS_H
#include <stdint.h>

/* prints the memory where the font and ROM are loaded at */
void print_program_memory(struct chip8_sys* chip8);

/* Push and pop from the stack */
uint16_t pop(struct state* s);
void push(struct state* s, uint16_t x);
#endif