#include "../include/termbox2/termbox.h"
#include "chip.h"

void draw_box_in_term_window(void){

    int height = tb_height();
    int width  = tb_width();

    for(int y = 0; y < height+1; y++)
    {
        for(int x = 0; x < width; x++)
        {
            /* draws the horizonal line at top and bottom */
            if(y == 0 || y == height-1)
                tb_set_cell(x, y, '=', TB_BLACK, TB_CYAN);
            
            /* draws the side bars */
            if(y > 0 && y < height-1){
                /* bar on the left */
                tb_set_cell(0, y, '=', TB_BLACK, TB_CYAN);
                
                /* bar on the right */
                tb_set_cell(width-1, y, '=', TB_BLACK, TB_CYAN);
            }
        }
    }
}

void draw_register_view(struct chip8_sys* chip8)
{
    (void) chip8;
}

int start_debugger(struct chip8_sys* chip8)
{
    /* initialise termbox2 */
    if(tb_init() != TB_OK){
        puts("Unable to init termbox2");
        return 1;
    }

    // remove this when actually using chip8
    (void) chip8;

    /* event structure */
    struct tb_event event;
    
    /* draws a box in the terminal window */
    tb_clear();
    draw_box_in_term_window();
    tb_present();
    

    uint8_t run = TRUE;
    while(run)
    {
        /* future keyboard controls here - CTRL-Q to exit */
        tb_poll_event(&event);
        if(event.key == TB_KEY_CTRL_Q)
            run = FALSE;
    }

    if(tb_shutdown() != TB_OK)
    {
        puts("Unable to shutdown termbox2");
        return 1;
    }

    return 0;
}