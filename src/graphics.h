#ifndef REBORN_CHIP_GRAPHICS_H
#define REBORN_CHIP_GRAPHICS_H

#include "chip.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>

/* Creates a window of height*width resolution. SDL Objects struct is passed to
 * it to access various variables for video rendering
 */
int create_window(unsigned int height, unsigned int width,
                  struct sdl_objs* sdl_objs);

void sdl_video_cleanup(struct sdl_objs* sdl_objs);
#endif