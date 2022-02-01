#include "SDL_atomic.h"
#include "SDL_log.h"
#include "SDL_mutex.h"
#include "SDL_thread.h"
#include "SDL_timer.h"
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <SDL2/SDL.h>

struct chip8_sys {

    const enum constants {
        MEMSIZE = 4096,
        REGNUM = 16,
        STACKSIZE = 48,
        DISPLAY_SIZE = 64 * 32
    } constants;

    uint16_t memory[MEMSIZE];
    uint16_t stack[STACKSIZE];
    uint8_t registers[REGNUM];
    uint8_t display[DISPLAY_SIZE];

    uint16_t index;
    uint16_t program_counter;
    uint8_t delay_timer;
    uint8_t sound_timer;

    int stacktop;
};

enum RUN { FALSE, TRUE };

enum MUTEX { DELAY_TIMER, SOUND_TIMER, DISPLAY };

struct state {
    struct chip8_sys* chip8;
    SDL_mutex** mutexes;
    SDL_atomic_t run;
};

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
    enum fread_param { inst_byte = 2 };

    if (fread(&chip8->memory[0x200], inst_byte, file_size, fp) !=
        (unsigned long)(file_size / inst_byte)) {
        puts("Error while reading rom to memory");
        return EXIT_FAILURE;
    }

    fclose(fp);
    return file_size;
}

static int delay_timer_thread(void* arg)
{
    struct state* state = (struct state*)arg;

    /* sleep for 17 milliseconds then continue - this is equivalent of
     * running the loop at 60hz While not accounting for locking and unlocking
     * mutexes, If the delay is changed the error range is too large Currently
     * we're off by 1 i.e if delay timer is set to 60, when the thread exits
     * after a minute the remaining value in delay timer is 1 */
    while (SDL_AtomicGet(&state->run)) {
        if (SDL_LockMutex(state->mutexes[DELAY_TIMER]) == 0) {
            state->chip8->delay_timer--;
            SDL_UnlockMutex(state->mutexes[DELAY_TIMER]);
            SDL_Delay(17);
        }
    }

    return TRUE;
}

static int sound_timer_thread(void* arg)
{
    struct state* state = (struct state*)arg;

    while (SDL_AtomicGet(&state->run)) {
        if (SDL_LockMutex(state->mutexes[SOUND_TIMER]) == 0) {
            state->chip8->sound_timer--;
            SDL_UnlockMutex(state->mutexes[SOUND_TIMER]);
            SDL_Delay(17);
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
    chip8.delay_timer = 60;
    chip8.sound_timer = 60;

    /* fetchrom and do other stuff */
    int file_size = fetchrom(&chip8, argv[1]);

    if (file_size == EXIT_FAILURE) {
        exit(EXIT_FAILURE);
    }

    printf("Rom: %s\nRom Size: %d Bytes\n", argv[1], file_size);

    /* Mutexes */
    SDL_mutex*(mutexes[]) = {[DELAY_TIMER] = SDL_CreateMutex(),
                             [SOUND_TIMER] = SDL_CreateMutex(),
                             [DISPLAY] = SDL_CreateMutex()};

    /* Populate the state struct */
    static struct state state = {.chip8 = &chip8};
    state.mutexes = mutexes;
    SDL_AtomicSet(&state.run, TRUE);

    /* Create a seprate thread for delay timer */
    int dt_thread_rtval, st_thread_rtval;
    SDL_Thread* dt_thread =
        SDL_CreateThread(delay_timer_thread, "DelayTimerThread", (void*)&state);
    SDL_Thread* st_thread =
        SDL_CreateThread(sound_timer_thread, "SoundTimerThread", (void*)&state);
    if (dt_thread == NULL || st_thread == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Could not create timer threads: %s", SDL_GetError());
    }

    /* Testing stuff - pauses main thread for 1 second and then set run to false
     */
    SDL_Delay(1000);
    SDL_AtomicSet(&state.run, FALSE);
    SDL_WaitThread(dt_thread, &dt_thread_rtval);
    SDL_WaitThread(st_thread, &st_thread_rtval);

    for (int i = 0; i < 3; i++)
        SDL_DestroyMutex(state.mutexes[i]);

    printf("Delay timer: %d\n", chip8.delay_timer);
    printf("Sound timer: %d\n", chip8.sound_timer);

    return 0;
}