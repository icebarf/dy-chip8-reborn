#include "chip.h"
#include "chip_instructions.h"
#include "helpers.h"
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_thread.h>
#include <stdint.h>
#include <stdlib.h>

/* loads the font to memory at address 0x0-0x50 (0 to 80)
 * the font is copied byte-by-byte
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
        puts("Unable to open rom");
        return EXIT_FAILURE;
    }

    /* seek to the end of file */
    if (fseek(fp, 0L, SEEK_END) != 0) {
        puts("Unable to SEEK to end of rom file");
        return EXIT_FAILURE;
    }

    /* store the position/byte of end */
    int file_size = ftell(fp);

    /* goes back */
    if (fseek(fp, 0L, SEEK_SET) != 0) {
        puts("Unable to return to where we SEEK'd from");
        return EXIT_FAILURE;
    }

    /* each chip8 instruction is two bytes long */
    enum fread_param { inst_byte = 1 };

    if (fread(&chip8->memory[0x200], inst_byte, file_size, fp) !=
        (unsigned long)(file_size / inst_byte)) {
        puts("Error while reading rom to memory");
        return EXIT_FAILURE;
    }

    fclose(fp);
    return file_size;
}

/* this function is supposed to run as a separate thread
 * it receives an arg which is actually the state structure
 * It continues to decrement the delay_timer until 0 and
 * then it stops decrementing - also exits when state->run is set to FALSE by
 * main thread
 */
static int delay_timer_thread(void* arg)
{
    if (arg == NULL)
        abort();

    struct state* state = (struct state*)arg;

    /* sleep for 17 milliseconds then continue - this is equivalent of
     * running the loop at 60hz ignoring the delay of function calls
     */
    while (SDL_AtomicGet(&state->run)) {
        uint8_t is_zero = (SDL_AtomicGet(&state->chip8->delay_timer) == 0);
        if (!is_zero) {
            SDL_AtomicDecRef(&state->chip8->delay_timer);
            SDL_Delay(17);
        }
    }

    return TRUE;
}

/* same as delay timer except it is for the sound timer */
static int sound_timer_thread(void* arg)
{
    if (arg == NULL)
        abort();

    struct state* state = (struct state*)arg;

    while (SDL_AtomicGet(&state->run)) {
        uint8_t is_zero = (SDL_AtomicGet(&state->chip8->sound_timer) == 0);
        if (!is_zero) {
            SDL_AtomicDecRef(&state->chip8->sound_timer);
            SDL_Delay(17);
        }
    }

    return TRUE;
}

/* fetch instruction
 * To know what all these random names mean
 * check src/chip.h - L11:22 */
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
    switch (s->ops->inst) {
    case 0x0:
        switch (s->ops->NN) {

        case 0xE0:
            break;

        case 0xEE:
            break;
        }
        break;

    case 0x1:
        break;

    case 0x2:
        break;

    case 0x3:
        break;

    case 0x4:
        break;

    case 0x5:
        break;

    case 0x6:
        break;

    case 0x7:
        break;

    case 0x8:
        switch (s->ops->N) {

        case 0x0:
            break;

        case 0x1:
            break;

        case 0x2:
            break;

        case 0x3:
            break;

        case 0x4:
            break;

        case 0x5:
            break;

        case 0x6:
            break;

        case 0x7:
            break;

        case 0xE:
            break;
        }
        break;

    case 0x9:
        break;

    case 0xA:
        break;

    case 0xB:
        break;

    case 0xC:
        break;

    case 0xD:
        break;

    case 0xE:
        switch (s->ops->NN) {

        case 0x9E:
            break;

        case 0xA1:
            break;
        }
        break;

    case 0xF:
        switch (s->ops->NN) {

        case 0x07:
            break;

        case 0x0A:
            break;

        case 0x15:
            break;

        case 0x18:
            break;

        case 0x1E:
            break;

        case 0x29:
            break;

        case 0x33:
            break;

        case 0x55:
            break;

        case 0x65:
            break;
        }
        break;

    default:
        SDL_Log("Chip8: Invalid Instruction detected");
        abort();
    }
}

static int emulator_thread(void* arg)
{
    if (arg == NULL)
        abort();

    struct state* state = (struct state*)arg;

    while (SDL_AtomicGet(&state->run)) {
        fetch(state);
        decode_execute(state);
    }

    return TRUE;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        puts("Usage: chip8 romfile.ch8");
        return 0;
    }

    /* initiate chip8 instance */
    static struct chip8_sys chip8 = {0};

    chip8.stacktop = -1;
    load_font(&chip8);
    chip8.program_counter = 0x200;

    /* Testing code */
    SDL_AtomicSet(&chip8.delay_timer, 60);
    SDL_AtomicSet(&chip8.sound_timer, 60);

    /* fetchrom and do other stuff */
    int file_size = fetchrom(&chip8, argv[1]);

    if (file_size == EXIT_FAILURE) {
        exit(EXIT_FAILURE);
    }

    printf("Rom: %s\nRom Size: %d Bytes\n", argv[1], file_size);

    /* Populate the state struct */
    static struct state state = {.chip8 = &chip8};
    SDL_AtomicSet(&state.run, TRUE);

    /* Create a seprate thread for delay timer, sound timer
     * and the main emulator loop */
    int dt_thread_rtval, st_thread_rtval, emu_thread_rtval;

    SDL_Thread* dt_thread =
        SDL_CreateThread(delay_timer_thread, "DelayTimerThread", (void*)&state);

    SDL_Thread* st_thread =
        SDL_CreateThread(sound_timer_thread, "SoundTimerThread", (void*)&state);

    SDL_Thread* emu_thread =
        SDL_CreateThread(emulator_thread, "EmuThread", (void*)&state);

    if (dt_thread == NULL || st_thread == NULL || emu_thread == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Could not create threads: %s", SDL_GetError());
    }

    /* On exit */
    SDL_AtomicSet(&state.run, FALSE);
    SDL_WaitThread(dt_thread, &dt_thread_rtval);
    SDL_WaitThread(st_thread, &st_thread_rtval);
    SDL_WaitThread(emu_thread, &emu_thread_rtval);

    /* print out whats left in the delay and sound timer*/
    printf("Delay timer: %d\n", SDL_AtomicGet(&chip8.delay_timer));
    printf("Sound timer: %d\n", SDL_AtomicGet(&chip8.sound_timer));

    return 0;
}
