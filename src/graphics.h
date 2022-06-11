#ifndef REBORN_CHIP_GRAPHICS_H
#define REBORN_CHIP_GRAPHICS_H

#include "chip.h"

/**
 * Creates a window of height*width resolution.
 * Also returns a sdl_objs structure defined in chip.h
 * sdl_objs structure contains the following data
 **
 * SDL_Window
 * SDL_Renderer
 * SDL_Texture
 * A Pixels array of 2048 of type uint32_t
 * A uint32_t value representing a RGBA Color value for each pixel
 **/
struct sdl_objs create_window(const unsigned int height, const unsigned int width, const uint32_t bg);

/**
 * Receives a SDL Objects structure
 * Destroyes all 'SDL_x' objects with respective 'SDL_DestroyX' function
 * free()s the pixels array
 **/
void video_cleanup(struct sdl_objs* sdl_objs);
#endif
