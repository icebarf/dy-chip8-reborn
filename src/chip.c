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

    if (fread(&chip8->memory[0x200], inst_byte, file_size, fp) !=
        (unsigned long)(file_size / inst_byte)) {
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
    s->ops->opcode = (s->chip8->memory[s->chip8->program_counter] << 8) |
                     s->chip8->memory[s->chip8->program_counter + 1];

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

void draw_to_display(struct state* s)
{

    for (uint16_t i = 0; i < DISPW * DISPH; i++) {
        if (s->chip8->display[i])
            s->sdl_objs->pixels[i] = s->data->fg;
        else
            s->sdl_objs->pixels[i] = s->data->bg;
    }

    SDL_RenderClear(s->sdl_objs->renderer);
    SDL_UpdateTexture(s->sdl_objs->texture, NULL, s->sdl_objs->pixels,
                      DISPW * sizeof(*s->sdl_objs->pixels));
    SDL_RenderCopy(s->sdl_objs->renderer, s->sdl_objs->texture, NULL, NULL);
    SDL_RenderPresent(s->sdl_objs->renderer);

    s->DrawFL = FALSE;
}

struct state initialise_emulator(struct chip8_sys* chip8,
                                 struct sdl_objs* sdl_objs, struct ops* op,
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

    /* Somehow need to efficiently pack these together
     * currently just yolo'ing it */
    void* ptrs[] = {
        [0x0] = &&nested_zero, [0x1] = &&one,   [0x2] = &&two,
        [0x3] = &&three,       [0x4] = &&four,  [0x5] = &&five,
        [0x6] = &&six,         [0x7] = &&seven, [0x8] = &&nested_eight,
        [0x9] = &&nine,        [0xA] = &&A,     [0xB] = &&B,
        [0xC] = &&C,           [0xD] = &&D,     [0xE] = &&nested_e,
        [0xF] = &&nested_f};

    void* ptrs_0[] = {[0xE0] = &&E0, [0xEE] = &&EE};

    void* ptrs_8[] = {
        [0x0] = &&eight_zero,  [0x1] = &&eight_one,   [0x2] = &&eight_two,
        [0x3] = &&eight_three, [0x4] = &&eight_four,  [0x5] = &&eight_five,
        [0x6] = &&eight_six,   [0x7] = &&eight_seven, [0xE] = &&eight_e};

    void* ptrs_e[] = {[0x9E] = &&E_9E, [0xA1] = &&E_A1};

    void* ptrs_f[] = {[0x07] = &&F_07, [0x0A] = &&F_0A, [0x15] = &&F_15,
                      [0x18] = &&F_18, [0x1E] = &&F_1E, [0x29] = &&F_29,
                      [0x33] = &&F_33, [0x55] = &&F_55, [0x65] = &&F_65};

    while (state->run == TRUE) {
        /* Timing counters */
        state->current_counter_val = SDL_GetPerformanceCounter();
        state->delta_time = get_delta_time(state->current_counter_val,
                                           state->previous_counter_val);
        state->delta_accumulation += state->delta_time;
        state->previous_counter_val = state->current_counter_val;

        fetch(state);
        goto* ptrs[state->ops->inst_nib];

    nested_zero:
        goto* ptrs_0[state->ops->NN];

    nested_eight:
        goto* ptrs_8[state->ops->N];

    nested_e:
        goto* ptrs_e[state->ops->NN];

    nested_f:
        goto* ptrs_f[state->ops->NN];

    E0:
        instruction_00e0(state);
        goto end;

    EE:
        instruction_00ee(state->chip8);
        goto end;

    one:
        instruction_1nnn(state->chip8, state->ops);
        goto end;

    two:
        instruction_2nnn(state->chip8, state->ops);
        goto end;

    three:
        instruction_3xnn(state->chip8, state->ops);
        goto end;

    four:

        instruction_4xnn(state->chip8, state->ops);
        goto end;

    five:
        instruction_5xy0(state->chip8, state->ops);
        goto end;

    six:
        instruction_6xnn(state->chip8, state->ops);
        goto end;

    seven:
        instruction_7xnn(state->chip8, state->ops);
        goto end;

    eight_zero:
        instruction_8xy0(state->chip8, state->ops);
        goto end;

    eight_one:
        instruction_8xy1(state->chip8, state->ops);
        goto end;

    eight_two:
        instruction_8xy2(state->chip8, state->ops);
        goto end;

    eight_three:
        instruction_8xy3(state->chip8, state->ops);
        goto end;

    eight_four:
        instruction_8xy4(state->chip8, state->ops);
        goto end;

    eight_five:
        instruction_8xy5(state->chip8, state->ops);
        goto end;

    eight_six:
        instruction_8xy6(state->chip8, state->ops, state->data);
        goto end;

    eight_seven:
        instruction_8xy7(state->chip8, state->ops);
        goto end;

    eight_e:
        instruction_8xye(state->chip8, state->ops, state->data);
        goto end;

    nine:
        instruction_9xy0(state->chip8, state->ops);
        goto end;

    A:
        instruction_annn(state->chip8, state->ops);
        goto end;

    B:
        instruction_bnnn(state->chip8, state->ops);
        goto end;

    C:
        instruction_cxnn(state->chip8, state->ops);
        goto end;

    D:
        instruction_dxyn(state);
        goto end;

    E_9E:
        instruction_ex9e(state);
        goto end;

    E_A1:
        instruction_exa1(state);
        goto end;

    F_07:
        instruction_fx07(state->chip8, state->ops);
        goto end;

    F_0A:
        instruction_fx0a(state);
        goto end;

    F_15:
        instruction_fx15(state->chip8, state->ops);
        goto end;

    F_18:
        instruction_fx18(state->chip8, state->ops);
        goto end;

    F_1E:
        instruction_fx1e(state->chip8, state->ops);
        goto end;

    F_29:
        instruction_fx29(state->chip8, state->ops);
        goto end;

    F_33:
        instruction_fx33(state->chip8, state->ops);
        goto end;

    F_55:
        instruction_fx55(state->chip8, state->ops, state->data);
        goto end;

    F_65:
        instruction_fx65(state->chip8, state->ops, state->data);
        goto end;

    end : {
    }

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
        fprintf(stderr, RED "Could not init SDL Video: %s\n" RESET,
                SDL_GetError());
        return BAD_RETURN_VALUE;
    }

    /* Initilaise the structures */
    static struct chip8_sys chip8 = {.memory = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
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
