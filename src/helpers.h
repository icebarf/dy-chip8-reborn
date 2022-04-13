#ifndef REBORN_HELPERS_H
#define REBORN_HELPERS_H
#include <stdint.h>

uint16_t pop(struct chip8_sys* chip8);
void push(struct chip8_sys* chip8, const uint16_t x);

#endif
