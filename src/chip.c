#include "chip.h"
#include "chip_instructions.h"
#include "graphics.h"
#include "helpers.h"
#include "keyboard.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_atomic.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_mutex.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_timer.h>

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* loads the font to memory at address 0x0-0x50 (0 to 80)
 */
static void load_font(struct chip8_sys* chip8)
{
    enum sprite_count { sp_count = 80 };

    uint8_t font[] = {
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
    };

    memcpy(&chip8->memory[0], &font[0], sp_count);
}

/* loads the specified rom to memory at adress 0x200 (512)
 * returns the number of character that are read
 */
static int fetchrom(struct chip8_sys* chip8, const char* name)
{
    FILE* fp = fopen(name, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Unable to find rom: %s\n", name);
        return -1;
    }

    /* seek to the end of file */
    if (fseek(fp, 0L, SEEK_END) != 0) {
        fprintf(stderr, "Unable to SEEK to end of rom file");
        return -1;
    }

    int file_size = ftell(fp);

    /* goes back */
    if (fseek(fp, 0L, SEEK_SET) != 0) {
        fprintf(stderr, "Unable to return to where we SEEK'd from");
        return -1;
    }

    /* each chip8 instruction is two bytes long */
    enum fread_param { inst_byte = 1 };

    if (fread(&chip8->memory[0x200], inst_byte, file_size, fp) !=
        (unsigned long)(file_size / inst_byte)) {
        fprintf(stderr, "Error while reading rom to memory");
        return -1;
    }

    fclose(fp);
    return file_size;
}

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
            instruction_8xy6(s->chip8, s->ops);
            break;

        case 0x7:
            instruction_8xy7(s->chip8, s->ops);
            break;

        case 0xE:
            instruction_8xye(s->chip8, s->ops);
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
            instruction_fx55(s->chip8, s->ops);
            break;

        case 0x65:
            instruction_fx65(s->chip8, s->ops);
            break;
        }
        break;

    default:
        SDL_Log("Chip8: Invalid Instruction detected\nOpcode: %x",
                s->ops->opcode);
    }
}

static int timer_thread(void* arg)
{
    assert(arg != NULL);

    struct atoms* atomics = (struct atoms*)arg;

    while (SDL_AtomicGet(atomics->run)) {
        uint8_t zero = SDL_AtomicGet(atomics->timer) == 0;

        if (!zero) {
            SDL_AtomicDecRef(atomics->timer);
            SDL_Delay(17);
        }
    }

    return TRUE;
}

void draw_to_display_thread(struct state* s)
{

    for (uint16_t i = 0; i < DISPW * DISPH; i++) {
        if (s->chip8->display[i])
            s->sdl_objs->pixels[i] = 0x61afefff;
        else
            s->sdl_objs->pixels[i] = 0x282c34ff;
    }

    SDL_RenderClear(s->sdl_objs->renderer);

    SDL_UpdateTexture(s->sdl_objs->texture, NULL, s->sdl_objs->pixels,
                      DISPW * sizeof(*s->sdl_objs->pixels));

    SDL_RenderCopy(s->sdl_objs->renderer, s->sdl_objs->texture, NULL, NULL);

    SDL_RenderPresent(s->sdl_objs->renderer);

    s->DrawFL = FALSE;
}

void emulator(struct state* state)
{
    while (SDL_AtomicGet(&state->run) == TRUE) {
        fetch(state);
        decode_execute(state);

        SDL_Event event;
        SDL_PollEvent(&event);
        int keycnt = 0;

        switch (event.type) {
        case SDL_QUIT:
            SDL_AtomicSet(&state->run, FALSE);
            break;

        case SDL_KEYUP:
            check_and_modify_keystate(SDL_GetKeyboardState(&keycnt), state);
            break;

        case SDL_KEYDOWN:
            check_and_modify_keystate(SDL_GetKeyboardState(&keycnt), state);
            break;
        }
        if (state->DrawFL)
            draw_to_display_thread(state);
        // implement this quirk
        SDL_Delay(01);
    }
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        puts("Usage: chip8 romfile.ch8");
        return 0;
    }
    srand(time(NULL));

    static struct chip8_sys chip8 = {0};
    chip8.stacktop = -1;
    load_font(&chip8);

    /* Programs have write access in memory from address 512(0x200) */
    chip8.program_counter = 0x200;

    /* Emulator Initialisation */
    SDL_AtomicSet(&chip8.delay_timer, 0);
    SDL_AtomicSet(&chip8.sound_timer, 0);

    int file_size = fetchrom(&chip8, argv[1]);
    if (file_size == -1) {
        return -1;
    }

    /* initialise video*/
    struct sdl_objs sdl_objs = {0};

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        fprintf(stderr, "Could not init SDL Video: %s\n", SDL_GetError());
        return -1;
    }

    if (create_window(DISPH * 15, DISPW * 15, &sdl_objs)) {
        return -1;
    }

    /* Populate the state struct */
    struct ops op = {0};
    static struct state state = {.chip8 = &chip8, .keystates = {0}};
    state.sdl_objs = &sdl_objs;
    state.ops = &op;
    SDL_AtomicSet(&state.run, TRUE);
    state.DrawFL = FALSE;

    /* Threads */
    int st_thread_rtval, dt_thread_rtval;
    SDL_Thread *st_thread, *dt_thread;

    /* for timer threads */
    struct atoms atomics_dt = {.run = &state.run, .timer = &chip8.delay_timer};
    struct atoms atomics_st = {.run = &state.run, .timer = &chip8.sound_timer};

    /* delay timer*/
    if ((dt_thread = SDL_CreateThread(timer_thread, "DelayTimerThread",
                                      (void*)&atomics_dt)) == NULL) {

        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Could not create delay timer thread: %s", SDL_GetError());
        SDL_AtomicSet(&state.run, FALSE);
        return -1;
    }

    /* sound timer */
    if ((st_thread = SDL_CreateThread(timer_thread, "DelayTimerThread",
                                      (void*)&atomics_st)) == NULL) {

        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Could not create sound timer thread: %s", SDL_GetError());
        SDL_AtomicSet(&state.run, FALSE);
        return -1;
    }

    /* Run the emulator */
    emulator(&state);

    /* On exit */
    SDL_WaitThread(dt_thread, &dt_thread_rtval);
    SDL_WaitThread(st_thread, &st_thread_rtval);
    video_cleanup(&sdl_objs);
    return 0;
}
