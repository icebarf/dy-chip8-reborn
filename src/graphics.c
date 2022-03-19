#include "graphics.h"
#include "chip.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int create_window(unsigned int height, unsigned int width,
                  struct sdl_objs* sdl_objs)
{
    /* initialise video*/
    if (SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Could not init SDL Video: %s\n", SDL_GetError());
        return 1;
    }

    /* create a window centered at screen of resolution width*height */
    sdl_objs->screen =
        SDL_CreateWindow("Chip-8 Reborn", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, width, height, 0);

    if (sdl_objs->screen == NULL) {
        fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    /* create a software based renderer on 'screen' */
    sdl_objs->renderer =
        SDL_CreateRenderer(sdl_objs->screen, -1, SDL_RENDERER_SOFTWARE);
    if (sdl_objs->renderer == NULL) {
        fprintf(stderr, "Could not create render: %s\n", SDL_GetError());
        return 1;
    }

    /* RGBA color on renderer - 0 0 0 1 -- while also check the return value */
    if (SDL_SetRenderDrawColor(sdl_objs->renderer, 0, 0, 0, 1) < 0) {
        fprintf(stderr, "Could not set render color: %s\n", SDL_GetError());
        return 1;
    }

    /* clear the render with above color */
    if (SDL_RenderClear(sdl_objs->renderer) < 0) {
        fprintf(stderr, "Could not clear render on screen: %s\n",
                SDL_GetError());
        return 1;
    }

    /* Texture - A texture with pixels of ABGR format(32 bits) of DISPW * DISPH.
     * Note that DISPW and DISPH can and mostly will be different from 'height'
     * and 'width' because while we may want to create a window of larger
     * resolution but the texture should be of the original chip8 display size.
     * While this can be generally treated with other methods which i will not
     * go detail into, I prefer the simple to implement (yet very blocky)
     * approach */
    sdl_objs->texture =
        SDL_CreateTexture(sdl_objs->renderer, SDL_PIXELFORMAT_ABGR32,
                          SDL_TEXTUREACCESS_STREAMING, DISPW, DISPH);

    if (sdl_objs->texture == NULL) {
        fprintf(stderr, "Could not create texture: %s\n", SDL_GetError());
        return 1;
    }

    /* Set pixels to an initial color  - I think this is unnecessary but I'll do
     * it just this once, until I deem it completely unncessary */
    uint32_t* pixels = malloc(DISPW * DISPH * sizeof(uint32_t));
    for (int i = 0; i < DISPH * DISPW; i++)
        pixels[i] = 0x00082032;

    /* Reuse this pixel memory somewhere else in the program */
    sdl_objs->pixels = pixels;

    /* update the texture - perform error checking as well */
    if (SDL_UpdateTexture(sdl_objs->texture, NULL, pixels,
                          DISPH * sizeof(uint32_t))) {
        fprintf(stderr, "Couldn't update texture: %s\n", SDL_GetError());
        return 1;
    }

    /* Copy texture to render */
    if (SDL_RenderCopy(sdl_objs->renderer, sdl_objs->texture, NULL, NULL) < 0) {
        fprintf(stderr, "Couldn't copy texture to render: %s\n",
                SDL_GetError());
        return 1;
    }

    /* present the render */
    SDL_RenderPresent(sdl_objs->renderer);

    return 0;
}

/* Cleans up sdl video subsystem and any related allocated memory */
void sdl_video_cleanup(struct sdl_objs* sdl_objs)
{
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    free(sdl_objs->pixels);
}