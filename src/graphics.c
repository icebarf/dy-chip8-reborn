#include "graphics.h"
#include "chip.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_atomic.h>
#include <SDL2/SDL_mutex.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int create_window(unsigned int height, unsigned int width,
                  struct sdl_objs* sdl_objs)
{
    /* Create window */
    sdl_objs->screen =
        SDL_CreateWindow("Chip-8 Reborn", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, width, height, 0);

    if (sdl_objs->screen == NULL) {
        fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    /* Create renderer on window */
    sdl_objs->renderer =
        SDL_CreateRenderer(sdl_objs->screen, -1, SDL_RENDERER_SOFTWARE);

    if (sdl_objs->renderer == NULL) {
        fprintf(stderr, "Could not create render: %s\n", SDL_GetError());
        return 1;
    }

    if (SDL_SetRenderDrawColor(sdl_objs->renderer, 0, 0, 0, 1) < 0) {
        fprintf(stderr, "Could not set render color: %s\n", SDL_GetError());
        return 1;
    }

    if (SDL_RenderClear(sdl_objs->renderer) < 0) {
        fprintf(stderr, "Could not clear render on screen: %s\n",
                SDL_GetError());
        return 1;
    }

    /* Create a texture in ABGR32 format
     * AGBR32 because We're on left endian machine so when setting pixels
     * we can simply write the hex of the color in RGBA format */
    sdl_objs->texture =
        SDL_CreateTexture(sdl_objs->renderer, SDL_PIXELFORMAT_ABGR32,
                          SDL_TEXTUREACCESS_STREAMING, DISPW, DISPH);

    if (sdl_objs->texture == NULL) {
        fprintf(stderr, "Could not create texture: %s\n", SDL_GetError());
        return 1;
    }

    /* Set pixel color on scren, also store the pixels array in sdl_objs
     * so it can be used elsewhere */
    uint32_t* pixels = malloc(DISPW * DISPH * sizeof(uint32_t));

    for (int i = 0; i < DISPH * DISPW; i++)
        pixels[i] = 0x282c34ff; // one-dark theme background

    sdl_objs->pixels = pixels;

    /* Update the texture and then copy the texture to renderer on window and
     * then present it */
    if (SDL_UpdateTexture(sdl_objs->texture, NULL, pixels,
                          DISPH * sizeof(uint32_t))) {
        fprintf(stderr, "Couldn't update texture: %s\n", SDL_GetError());
        return 1;
    }

    if (SDL_RenderCopy(sdl_objs->renderer, sdl_objs->texture, NULL, NULL) < 0) {
        fprintf(stderr, "Couldn't copy texture to render: %s\n",
                SDL_GetError());
        return 1;
    }

    SDL_RenderPresent(sdl_objs->renderer);
    return 0;
}

void video_cleanup(struct sdl_objs* sdl_objs, SDL_mutex* pixel_mutex)
{
    SDL_LockMutex(pixel_mutex);
    SDL_DestroyTexture(sdl_objs->texture);
    SDL_DestroyRenderer(sdl_objs->renderer);
    SDL_DestroyWindow(sdl_objs->screen);
    SDL_Quit();
    free(sdl_objs->pixels);
    SDL_UnlockMutex(pixel_mutex);
}
