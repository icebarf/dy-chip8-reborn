#include "helpers.h"

#include <SDL2/SDL_timer.h>
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CP_STRLEN(str) (sizeof(str) - 1)

size_t strnlen_rb(const char* str, size_t maxlen)
{
    size_t counter = 0;
    while ((counter < maxlen) && (*str != '\0')) {
        counter++;
        str++;
    }
    return counter;
}

uint16_t pop(struct chip8_sys* chip8)
{
    return chip8->stack[chip8->stacktop--];
}

void push(struct chip8_sys* chip8, const uint16_t x)
{
    chip8->stack[++chip8->stacktop] = x;
}

double get_delta_time(const double current, const double previous)
{
    return ((current - previous) * 1000000000.0) /
           SDL_GetPerformanceFrequency();
}

void print_help(void)
{
    // clang-format off
    puts(
        BOLD GREEN_2 "Dy-Chip8-Reborn\n"
        "Usage: chip8-rb [OPTIONS]\n\n" RESET
        BOLD "Options:\n" RESET
        "  --help             Shows this help page\n"
        "  --rom [PATH]       Specify path to chip8 ROM file\n"
        "  --quirks           Enables specific quirks in emulator\n"
        "  --freq             Specify the frequency at which the emulated cpu runs\n"
        "  --debug            Enables the debugger in the emulator to debug programs\n"
        "  --colors [BG] [FG] Specify the background and the foreground color\n"
        BOLD RED_2 "\nAdditional Notes:\n" RESET
        "  Quirks             There are instruction specific quirks that can be"
        "enabled by specifying the option.\n"
        "                     Most ROMs work well without the enable of these quirks.\n\n"
        "  Freq               The cpu frequency specified should be in megahertz.\n"
        "                     This option is ignored in this version of this emulator\n\n"
        "  Colors             The foreground and background colors should both be specified\n"
        "                     use hexadecimal base, append 'ff' at the end of your color's hex value\n");
    // clang-format on
}

void bad_arg(void)
{
    puts(RED_2 "dy-chip8: invalid usage.\n" RESET
               "Type 'dy-chip8 -h' for help");
    exit(EXIT_FAILURE);
}

void parse_argv(const int argc, const char** argv,
                struct chip8_launch_data* data)
{
    // clang-format off
    char* options[] = {
        "--help",
        "--rom",
        "--quirks",
        "--freq", // this will stay until I figure out a proper way to do cpu frequency
        "--debug",
        "--colors",
        "-h"
    };
    
    enum OPTIONS {
        HELP = 0,
        ROM = 1,
        QRK = 2,
        FRQ = 3,
        DBG = 4,
        COL = 5,
        HELP_2 = 6,

        HELP_L = CP_STRLEN("--help"),
        HELP_2_L = CP_STRLEN("-h"),
        ROM_L = CP_STRLEN("--rom"),
        QRK_L = CP_STRLEN("--quirks"),
        FRQ_L = CP_STRLEN("--freq"),
        DBG_L = CP_STRLEN("--debug"),
        COL_L = CP_STRLEN("--colors")
    };

    size_t index = 1;
    while (index < (size_t)argc) {
        if (strncmp(options[HELP], argv[index], strnlen_rb(argv[index], HELP_L)) == 0
            || strncmp(options[HELP_2], argv[index], strnlen_rb(argv[index], HELP_2_L)) == 0)
        {
            print_help();
            exit(EXIT_SUCCESS);
        }

        if (strncmp(options[ROM], argv[index], strnlen_rb(argv[index], ROM_L)) == 0) {
            index++;

            if(argv[index][0] == '-')
                bad_arg();

            data->rom_path = malloc(strlen(argv[index]) + 1);
            assert(data->rom_path != NULL);
            strncpy(data->rom_path, argv[index], strlen(argv[index]) + 1);
            data->yes_rom = TRUE;
            index++;
            
            continue;
        }

        if(strncmp(options[QRK], argv[index], strnlen_rb(argv[index], QRK_L)) == 0) {
            data->quirks = TRUE;
            index++;

            continue;
        }

        if(strncmp(options[DBG], argv[index], strnlen_rb(argv[index], DBG_L)) == 0) {
            data->debugger = TRUE;
            index++;

            continue;
        }

        if(strncmp(options[FRQ], argv[index], strnlen_rb(argv[index], FRQ_L)) == 0) {
            index++;
            if(argv[index][0] == '-')
                bad_arg();

            data->frequency = strtoul(argv[index], NULL, 10);
            assert(data->frequency != 0);
            index++;

            continue;
        }

        if(strncmp(options[COL], argv[index], strnlen_rb(argv[index], COL_L)) == 0) {
            index++;
            data->bg = strtol(argv[index], NULL, 16);
            index++;
            data->fg = strtol(argv[index], NULL, 16);
            index++;

            continue;
        }

        /* if nothing matches then bad argument*/
        bad_arg();
    }
    // clang-format on
}

void print_chip8_settings(const struct chip8_launch_data* data)
{
    // clang-format off
    printf(BOLD ULINE GREEN"\n[Chip-8 Reborn]\nEmulator Settings\n\n" RESET
        BLUE "%16s " RESET "- %15d\n"
        BLUE "%16s " RESET "- %s\n"
        BLUE "%16s " RESET "- %15x\n"
        BLUE "%16s " RESET "- %15x\n"
        BLUE "%16s " RESET "- %15lu Mhz\n"
        BLUE "%16s " RESET "- %15d\n"
        BLUE "%16s " RESET "- %15d\n"
        GREEN_2 BOLD "\nLegend - 0 for Disabled, 1 for Enabled\n" RESET,
        "Rom Available", data->yes_rom, "Rom Path", data->rom_path, "Fg",
           data->fg, "Bg", data->bg, "Frequency", data->frequency,
           "Qurks Enabled", data->quirks, "Debugger Enabled", data->debugger);
    // clang-format on
}

void debug_log(const char* string)
{
    fprintf(stdout, "chip8: %s", string);
}
