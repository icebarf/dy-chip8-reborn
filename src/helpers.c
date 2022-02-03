#include "chip.h"
#include "helpers.h"
#include <stdio.h>

void print_program_memory(struct chip8_sys *chip8)
{
    puts("Chip8 Struct Memory State");
    puts("=========================\n");
    puts("Font Memory Address\t    [col1]     [col2]      [col3]      [col4]      [col5]");
    for(int i = 0; i < 80; i+=5)
    {
        printf("0x%x-0x%x          \t\t0x%2x\t\t0x%2x\t\t0x%2x\t\t0x%2x\t\t0x%2x\n",i,i+4,chip8->memory[i],chip8->memory[i+1],chip8->memory[i+2],chip8->memory[i+3], chip8->memory[i+4]);
    }
    puts("=========================\n");
    puts("Address-Range\t\t[col1]\t\t[col2]\t\t[col3]\t\t[col4]");
    for(int i = 0x200; i < 0x400; i+=4)
    {
        printf("%x-%x\t\t\t\t0x%2x\t\t0x%2x\t\t0x%2x\t\t0x%2x\n",i,i+4,chip8->memory[i],chip8->memory[i+1],chip8->memory[i+2],chip8->memory[i+3]);
    }
    puts("\n=========================");
}