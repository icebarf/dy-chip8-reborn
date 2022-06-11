#include "chip.h"
#include "chip_instructions.h"
#include "graphics.h"
#include "helpers.h"
#include "keyboard.h"

#include <SDL2/SDL_timer.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

/* loads the specified rom to memory at adress 0x200 (512)
 * returns the number of character that are read
 */
static int fetchrom(struct chip8_sys* chip8, const char* name)
{
    FILE* fp = fopen(name, "rb");
    if (fp == NULL) {
        debug_log(RED "Failed: Unable to find rom\n" RESET);
        return BAD_RETURN_VALUE;
    }

    /* seek to the end of file */
    if (fseek(fp, 0L, SEEK_END) != 0) {
        debug_log(RED "Failed: Seek to end of rom file\n" RESET);
        return BAD_RETURN_VALUE;
    }

    int file_size = ftell(fp);

    /* goes back */
    if (fseek(fp, 0L, SEEK_SET) != 0) {
        debug_log(RED "Failed: Seek back to previous position\n" RESET);
        return BAD_RETURN_VALUE;
    }

    /* each chip8 instruction is two bytes long */
    enum fread_param { inst_byte = 1 };

    if (fread(&chip8->memory[0x200], inst_byte, file_size, fp) != (unsigned long)(file_size / inst_byte)) {
        debug_log(RED "Failed: Reading ROM to emulator memory\n" RESET);
        return BAD_RETURN_VALUE;
    }

    fclose(fp);
    return file_size;
}

/**
 * fetches the instruction to be executed.
 * also sets the operands (ops) structure.
 * magic constants used here are different mask values to obtain
 * various required bits of the 16-bit opcode on which the instructions operate
 **/
void fetch(struct state* s)
{
    s->ops->opcode =
        (s->chip8->memory[s->chip8->program_counter] << 8) | s->chip8->memory[s->chip8->program_counter + 1];

    uint16_t tmp = (s->ops->opcode << 4) & 0xffff;
    s->ops->NNN = (tmp >> 4) & 0xffff;

    s->ops->NN = s->chip8->memory[s->chip8->program_counter + 1];

    s->ops->inst = s->ops->opcode >> 8 & 0xff;

    s->ops->inst_nib = (s->ops->inst >> 4) & 0xff;

    s->ops->X = (s->ops->NNN >> 8) & 0xff;

    s->ops->Y = (s->ops->NN >> 4) & 0xff;

    tmp = (s->ops->NN << 4) & 0xff;
    s->ops->N = (tmp >> 4) & 0xff;

    /* increment the PC */
    s->chip8->program_counter += 2;
}

void decode_execute(struct state* s)
{
    switch (s->ops->inst_nib) {
        case 0x0:
            switch (s->ops->NN) {
                case 0xE0:
                    instruction_00e0(s);
                    break;

                case 0xEE:
                    instruction_00ee(s->chip8);
                    break;
            }
            break;

        case 0x1:
            instruction_1nnn(s->chip8, s->ops);
            break;

        case 0x2:
            instruction_2nnn(s->chip8, s->ops);
            break;

        case 0x3:
            instruction_3xnn(s->chip8, s->ops);
            break;

        case 0x4:
            instruction_4xnn(s->chip8, s->ops);
            break;

        case 0x5:
            instruction_5xy0(s->chip8, s->ops);
            break;

        case 0x6:
            instruction_6xnn(s->chip8, s->ops);
            break;

        case 0x7:
            instruction_7xnn(s->chip8, s->ops);
            break;

        case 0x8:
            switch (s->ops->N) {
                case 0x0:
                    instruction_8xy0(s->chip8, s->ops);
                    break;

                case 0x1:
                    instruction_8xy1(s->chip8, s->ops);
                    break;

                case 0x2:
                    instruction_8xy2(s->chip8, s->ops);
                    break;

                case 0x3:
                    instruction_8xy3(s->chip8, s->ops);
                    break;

                case 0x4:
                    instruction_8xy4(s->chip8, s->ops);
                    break;

                case 0x5:
                    instruction_8xy5(s->chip8, s->ops);
                    break;

                case 0x6:
                    instruction_8xy6(s->chip8, s->ops, s->data);
                    break;

                case 0x7:
                    instruction_8xy7(s->chip8, s->ops);
                    break;

                case 0xE:
                    instruction_8xye(s->chip8, s->ops, s->data);
                    break;
            }
            break;

        case 0x9:
            instruction_9xy0(s->chip8, s->ops);
            break;

        case 0xA:
            instruction_annn(s->chip8, s->ops);
            break;

        case 0xB:
            instruction_bnnn(s->chip8, s->ops);
            break;

        case 0xC:
            instruction_cxnn(s->chip8, s->ops);
            break;

        case 0xD:
            instruction_dxyn(s);
            break;

        case 0xE:
            switch (s->ops->NN) {
                case 0x9E:
                    instruction_ex9e(s);
                    break;

                case 0xA1:
                    instruction_exa1(s);
                    break;
            }
            break;

        case 0xF:
            switch (s->ops->NN) {
                case 0x07:
                    instruction_fx07(s->chip8, s->ops);
                    break;

                case 0x0A:
                    instruction_fx0a(s);
                    break;

                case 0x15:
                    instruction_fx15(s->chip8, s->ops);
                    break;

                case 0x18:
                    instruction_fx18(s->chip8, s->ops);
                    break;

                case 0x1E:
                    instruction_fx1e(s->chip8, s->ops);
                    break;

                case 0x29:
                    instruction_fx29(s->chip8, s->ops);
                    break;

                case 0x33:
                    instruction_fx33(s->chip8, s->ops);
                    break;

                case 0x55:
                    instruction_fx55(s->chip8, s->ops, s->data);
                    break;

                case 0x65:
                    instruction_fx65(s->chip8, s->ops, s->data);
                    break;
            }
            break;

        default:
            __builtin_unreachable();
    }
}

void draw_to_display(struct state* s)
{
    for (uint16_t i = 0; i < DISPW * DISPH; i++) {
        if (s->chip8->display[i])
            s->sdl_objs->pixels[i] = s->data->fg;
        else
            s->sdl_objs->pixels[i] = s->data->bg;
    }

    SDL_RenderClear(s->sdl_objs->renderer);
    SDL_UpdateTexture(s->sdl_objs->texture, NULL, s->sdl_objs->pixels, DISPW * sizeof(*s->sdl_objs->pixels));
    SDL_RenderCopy(s->sdl_objs->renderer, s->sdl_objs->texture, NULL, NULL);
    SDL_RenderPresent(s->sdl_objs->renderer);

    s->DrawFL = FALSE;
}

struct state initialise_emulator(struct chip8_sys* chip8,
                                 struct sdl_objs* sdl_objs,
                                 struct ops* op,
                                 struct chip8_launch_data* data)
{
    /* verify received arguements aren't NULL pointers */
    assert(chip8);
    assert(sdl_objs);
    assert(op);
    assert(data);

    /* Fill the state structure */
    struct state state = {0};

    state.chip8 = chip8;
    state.ops = op;
    state.run = TRUE;
    state.sdl_objs = sdl_objs;
    state.DrawFL = FALSE;
    state.data = data;

    /* chip8 structure initialisation */
    state.chip8->stacktop = INITIAL_STACK_TOP_LOCATION;
    state.chip8->program_counter = PROGRAM_LOAD_ADDRESS;

    if (fetchrom(chip8, data->rom_path) == BAD_RETURN_VALUE) {
        exit(1);
    }
    fprintf(stdout, GREEN_2 "\n\nLoaded Rom - %s\n" RESET, data->rom_path);

    /* sdl objects structure initialisation */
    *state.sdl_objs = create_window(DISPH * 15, DISPW * 15, data->bg);
    fprintf(stdout, GREEN_2 "Created window...\n" RESET);

    /* Current time seed for bad random
     * I do not really care if it is truly random */
    srand(time(NULL));

    return state;
}

void emulator(struct state* state)
{
    assert(state);

    while (state->run == TRUE) {
        /* Timing counters */
        state->current_counter_val = SDL_GetPerformanceCounter();
        state->delta_time = get_delta_time(state->current_counter_val, state->previous_counter_val);
        state->delta_accumulation += state->delta_time;
        state->previous_counter_val = state->current_counter_val;

        fetch(state);
        decode_execute(state);

        SDL_Event event;
        SDL_PollEvent(&event);

        switch (event.type) {
            case SDL_QUIT:
                state->run = FALSE;
                break;

            case SDL_KEYUP:
                check_and_modify_keystate(SDL_GetKeyboardState(NULL), state);
                break;

            case SDL_KEYDOWN:
                check_and_modify_keystate(SDL_GetKeyboardState(NULL), state);
                break;
        }

        if (state->DrawFL)
            draw_to_display(state);

        while (state->delta_accumulation >= TIMER_DEC_RATE) {
            if (state->chip8->delay_timer > 0)
                --state->chip8->delay_timer;

            if (state->chip8->sound_timer > 0)
                --state->chip8->sound_timer;

            state->delta_accumulation -= TIMER_DEC_RATE;
        }

        // implement this quirk - Soon @ Sun, 22 May 2022
        SDL_Delay(1);
    }
}
int main(int argc, char** argv)
{
    static struct chip8_launch_data data = {.quirks = FALSE,
                                            .yes_rom = FALSE,
                                            .debugger = FALSE,
                                            .rom_path = NULL,
                                            .bg = 0x282c34ff,
                                            .fg = 0x61afefff,
                                            .frequency = 1000};

    /* argument parsing */
    if (argc < 2) {
        bad_arg();
        return 0;

    } else {
        parse_argv(argc, (const char**)argv, &data);
        print_chip8_settings(&data);
        if (!data.yes_rom) {
            fprintf(stdout, RED_2 "chip8-rb: error: must specify rom\n" RESET);
            return 0;
        }
    }

    /* initialise video*/
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        fprintf(stderr, RED "Could not init SDL Video: %s\n" RESET, SDL_GetError());
        return BAD_RETURN_VALUE;
    }

    /* Initilaise the structures */
    static struct chip8_sys chip8 = {.memory = {
                                         0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
                                         0x20, 0x60, 0x20, 0x20, 0x70,  // 1
                                         0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
                                         0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
                                         0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
                                         0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
                                         0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
                                         0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
                                         0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
                                         0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
                                         0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
                                         0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
                                         0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
                                         0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
                                         0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
                                         0xF0, 0x80, 0xF0, 0x80, 0x80   // F
                                     }};
    static struct sdl_objs sdl_objs = {0};
    static struct ops op = {0};

    printf(GREEN BOLD ULINE "\n[Chip-8 Reborn]\nEmulator STATUS\n" RESET);

    struct state state = initialise_emulator(&chip8, &sdl_objs, &op, &data);

    /* Run the emulator */
    emulator(&state);

    /* On exit */
    video_cleanup(&sdl_objs);
    return 0;
}
