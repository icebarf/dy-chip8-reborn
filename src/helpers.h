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
double get_delta_time(const double current, const double previous);

/* outputs bad usage error to terminal*/
void bad_arg(void);

/* parses the options passed to the emulator and sets up the runtime structure
 */
void parse_argv(const int argc, const char** argv,
                struct chip8_launch_data* data);

/* prints out the chip8_launch_data structure to stdout */
void print_chip8_settings(const struct chip8_launch_data* data);

/* logs to terminal when DEBUG is defined */
void debug_log(const char* string);

#endif
