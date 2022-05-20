#include "chip.h"
#include <SDL2/SDL_timer.h>
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMP_STRLEN(str) (sizeof(str) - 1)

uint16_t pop(struct chip8_sys* chip8)
{
    return chip8->stack[chip8->stacktop--];
}

void push(struct chip8_sys* chip8, const uint16_t x)
{
    chip8->stack[++chip8->stacktop] = x;
}

double get_delta_time(const double current_counter_val,
                      const double prev_counter_val)
{
    return ((current_counter_val - prev_counter_val) * 1000000000.0) /
           SDL_GetPerformanceFrequency();
}

void print_help(void)
{
    puts("HELP!!!");
}

inline static void bad_arg(void)
{
    puts("dy-chip8: invalid usage.\nType 'dy-chip8 -h' for help");
    exit(EXIT_FAILURE);
}

void parse_argv(int argc, char** argv, struct chip8_launch_data* data)
{
    // clang-format off
    char* options[] = {
        "-h",
        "--rom",
        "--quirks",
        "--freq",
        "--debug"
    };
    
    enum OPTIONS {
        HELP = 0,
        ROM = 1,
        QRK = 2,
        FRQ = 3,
        DBG = 4,

        HELP_L = COMP_STRLEN("-h"),
        ROM_L = COMP_STRLEN("--rom"),
        QRK_L = COMP_STRLEN("--quirks"),
        FRQ_L = COMP_STRLEN("--freq"),
        DBG_L = COMP_STRLEN("--debug")
    };

    size_t index = 1;
    while (index < (size_t)argc) {
        if (strncmp(options[HELP], argv[index], strnlen(argv[index], ROM_L)) == 0)
        {
            print_help();
            exit(EXIT_SUCCESS);
        }

        if (strncmp(options[ROM], argv[index], strnlen(argv[index], ROM_L)) == 0) {
            index++;

            if(argv[index][0] == '-')
                bad_arg();

            data->rom_path = malloc(strlen(argv[index]));
            assert(data->rom_path != NULL);
            strncpy(data->rom_path, argv[index], strnlen(argv[index], ROM_L));
            data->yes_rom = TRUE;
            
            index++;
            continue;
        }

        if(strncmp(options[QRK], argv[index], strnlen(argv[index], QRK_L)) == 0) {
            data->quirks = TRUE;
            index++;

            continue;
        }

        if(strncmp(options[FRQ], argv[index], strnlen(argv[index], FRQ_L)) == 0) {
            index++;
            if(argv[index][0] == '-')
                bad_arg();

            data->frequency = strtoul(argv[index], NULL, 10);
            assert(data->frequency != 0);
            index++;

            continue;
        }

        if(strncmp(options[DBG], argv[index], strnlen(argv[index], DBG_L)) == 0) {
            data->debugger = TRUE;
            index++;

            continue;
        }
    }
    // clang-format on
}

void print_chip8_settings(struct chip8_launch_data* data)
{
    printf("\tChip8 Data:\n%s - %d\n%s - %d\n%s - %d\n%s - %s\n%s - %li\n",
           "quirks", data->quirks, "ROM Present", data->yes_rom, "Debug",
           data->debugger, "ROM", data->rom_path, "Frequency", data->frequency);
}