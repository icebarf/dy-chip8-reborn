#include "chip.h"
#include "debugger.h"
#include "helpers.h"
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_thread.h>
#include <stdint.h>

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

/* decode instruction starting with 0 */
int zero_inst(struct ops* o)
{
    switch (o->NN) {
    case 0xE0:
        return 0x1;
    case 0xEE:
        return 0x2;
    default:
        return 0x0;
    }

    return -1;
}

int eighth_inst(struct ops* o)
{
    switch (o->N) {
    case 0x0:
        return 0xA;

    case 0x1:
        return 0xB;

    case 0x2:
        return 0xC;

    case 0x3:
        return 0xD;

    case 0x4:
        return 0xE;

    case 0x5:
        return 0xF;

    case 0x6:
        return 0x10;

    case 0x7:
        return 0x11;

    case 0xE:
        return 0x12;
    }

    return -1;
}

int E_inst(struct ops* o)
{
    switch (o->NN) {
    case 0x9E:
        return 0x18;
    case 0xA1:
        return 0x19;
    }
    return -1;
}

int F_inst(struct ops* o)
{
    switch (o->NN) {

    case 0x07:
        return 0x1A;

    case 0x0A:
        return 0x1B;

    case 0x15:
        return 0x1C;

    case 0x18:
        return 0x1D;

    case 0x1E:
        return 0x1E;

    case 0x29:
        return 0x1F;

    case 0x33:
        return 0x20;

    case 0x55:
        return 0x21;

    case 0x65:
        return 0x22;
    }

    return -1;
}

int get_instruction_index(struct ops* o)
{

    switch (o->inst) {
    case 0x0:
        return zero_inst(o);

    case 0x1:
        return 0x3;

    case 0x2:
        return 0x4;

    case 0x3:
        return 0x5;

    case 0x4:
        return 0x6;

    case 0x5:
        return 0x7;

    case 0x6:
        return 0x8;

    case 0x7:
        return 0x9;

    case 0x8:
        return eighth_inst(o);

    case 0x9:
        return 0x13;

    case 0xA:
        return 0x14;
        break;

    case 0xB:
        return 0x15;
        break;

    case 0xC:
        return 0x16;
        break;

    case 0xD:
        return 0x17;
        break;

    case 0xE:
        return E_inst(o);
        break;

    case 0xF:
        return F_inst(o);
        break;
    }

    return -1;
}

int decode(struct state* s)
{
    return get_instruction_index(s->ops);
}

static int emulator_thread(void* arg)
{
    if (arg == NULL)
        abort();

    struct state* state = (struct state*)arg;

    while (SDL_AtomicGet(&state->run)) {
        fetch(state);
        int execute = decode(state);
        if (execute == -1) {
            SDL_Log("Chip8: Invalid Instruction detected");
            return FALSE;
        }
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

    /* Testing code */
    SDL_AtomicSet(&chip8.delay_timer, 60);
    SDL_AtomicSet(&chip8.sound_timer, 60);

    /* fetchrom and do other stuff */
    int file_size = fetchrom(&chip8, argv[1]);

    if (file_size == EXIT_FAILURE) {
        exit(EXIT_FAILURE);
    }

    printf("Rom: %s\nRom Size: %d Bytes\n", argv[1], file_size);

    /* Mutexes */
    SDL_mutex*(mutexes[]) = {[MEMORY] = SDL_CreateMutex()};

    /* Populate the state struct */
    static struct state state = {.chip8 = &chip8};
    state.mutexes = mutexes;
    SDL_AtomicSet(&state.run, TRUE);

    /* Create a seprate thread for delay timer, sound timer
     * and the main emulator loop */
    int dt_thread_rtval, st_thread_rtval, emu_thread_rtval;

    SDL_Thread* dt_thread =
        SDL_CreateThread(delay_timer_thread, "DelayTimerThread", (void*)&state);

    SDL_Thread* st_thread =
        SDL_CreateThread(sound_timer_thread, "SoundTimerThread", (void*)&state);
    if (dt_thread == NULL || st_thread == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Could not create timer threads: %s", SDL_GetError());
    }

    SDL_Thread* emu_thread =
        SDL_CreateThread(emulator_thread, "EmuThread", (void*)&state);

    /* On exit */
    SDL_Delay(1008);
    SDL_AtomicSet(&state.run, FALSE);
    SDL_WaitThread(dt_thread, &dt_thread_rtval);
    SDL_WaitThread(st_thread, &st_thread_rtval);

    for (int i = 0; i < 3; i++)
        SDL_DestroyMutex(state.mutexes[i]);

    /* print out whats left in the delay and sound timer*/
    printf("Delay timer: %d\n", SDL_AtomicGet(&chip8.delay_timer));
    printf("Sound timer: %d\n", SDL_AtomicGet(&chip8.sound_timer));

    /* Debugger - Should be on another thread or... perhaps not. Pending design
     * choice */
    start_debugger(&chip8);

    return 0;
}