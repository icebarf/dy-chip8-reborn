#ifndef REBORN_HELPERS_H
#define REBORN_HELPERS_H
#include <stdint.h>

/* takes in a pointer to chip8 instance, returns the topmost value from stack,
 * decrements the  stacktop */
uint16_t pop(struct chip8_sys* chip8);

/* takes in a pointer to chip8 instance, and value to-be-pushed onto the stack
 * first increments the stacktop and then stores the value at STACK[stacktop] */
void push(struct chip8_sys* chip8, const uint16_t x);

/* takes in the current and previous value of the SDL High Performance Counter
 * Calculates the delta (current - previous)
 * converts to nanoseconds and then returns the value */
double get_delta_time(const double current_counter_val, const double prev_counter_val);

#endif
