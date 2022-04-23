#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <raylib.h>
#define RAYLIB_H

#include "chip.h"
#include "chip_instructions.h"
#include "helpers.h"
#include "keyboard.h"
#include <assert.h>

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
        return BAD_RETURN_VALUE;
    }

    /* seek to the end of file */
    if (fseek(fp, 0L, SEEK_END) != 0) {
        fprintf(stderr, "Unable to SEEK to end of rom file");
        return BAD_RETURN_VALUE;
    }

    int file_size = ftell(fp);

    /* goes back */
    if (fseek(fp, 0L, SEEK_SET) != 0) {
        fprintf(stderr, "Unable to return to where we SEEK'd from");
        return BAD_RETURN_VALUE;
    }

    /* each chip8 instruction is two bytes long */
    enum fread_param { inst_byte = 1 };

    if (fread(&chip8->memory[0x200], inst_byte, file_size, fp) !=
        (unsigned long)(file_size / inst_byte)) {
        fprintf(stderr, "Error while reading rom to memory");
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
        TraceLog(LOG_ERROR,"Unknown instruction %x", s->ops->inst);
    }
}

void draw_to_display(struct state* s)
{
    ClearBackground((Color){42,42,42,255});
    for (uint16_t i = 0; i < DISPW * DISPH; i++) {
        Vector2 pos = { i % DISPW, i / DISPW };
        if (s->chip8->display[i]){
            //shaders would be cool but CPU rendering yay
            DrawRectangle(DISPSCALE*pos.x,DISPSCALE*pos.y,DISPSCALE,DISPSCALE,(Color){255,255,255,255});
        }
    }
    s->DrawFL = FALSE;
}

struct state initialise_emulator(const char* ROM, struct chip8_sys* chip8,
                                    struct ops* op){

    /* verify received arguements aren't NULL pointers */
    assert(ROM);
    assert(chip8);
    assert(op);

    /* Fill the state structure */
    struct state state = {0};

    state.chip8 = chip8;
    state.ops = op;
    state.run = TRUE;
    state.DrawFL = FALSE;

    /* chip8 structure initialisation */
    load_font(state.chip8);
    state.chip8->stacktop = INITIAL_STACK_TOP_LOCATION;
    state.chip8->program_counter = PROGRAM_LOAD_ADDRESS;
    if (fetchrom(chip8, ROM) == BAD_RETURN_VALUE) {
        exit(1);
    }
    InitWindow(DISPW * DISPSCALE, DISPH * DISPSCALE, "Icebarf Chip8 Emulator");
    //implement SDL Delay in framerate
    SetTargetFPS(60);
    /* Current time seed */
    srand(time(NULL));
    return state;
}

void emulator(struct state* state)
{
    assert(state);
    // loopity
    while(!WindowShouldClose()){
            state->delta_time = GetFrameTime(); 
            state->delta_accumulation = GetTime();

            /** Emulate one cycle **/
            fetch(state);
            decode_execute(state);
            check_and_modify_keystate(state);

            /* Raylib */
            BeginDrawing();
            if(state->DrawFL){
                draw_to_display(state);
            }
            EndDrawing();
    }
}

int main(int argc, char** argv){
    if (argc < 2) {
        puts("Usage: chip8 romfile.ch8");
        return 0;
    }

    /* Initilaise the structures */
    struct chip8_sys chip8 = {0};
    struct ops op = {0};

    struct state state = initialise_emulator(argv[1], &chip8, &op);

    /* Run the emulator */
    emulator(&state);

    /* On exit */
    CloseWindow();
    return 0;
}
