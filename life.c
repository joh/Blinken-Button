#include <string.h>
#include <avr/io.h>
#include <util/delay.h>

#include "display.h"
#include "life.h"

#define WIDTH  8
#define HEIGHT 8

uint8_t world[HEIGHT] = {0b00000000,
         0b00000000,
         0b00010100,
         0b00100000,
         0b00100010,
         0b00111100,
         0b00000000,
         0b00000000};

/**
 * Initialize Game of Life
 */
void life_init()
{
    /*uint8_t i;
    for (i = 0; i < HEIGHT; i++) {
        world[i] = 0;
    }*/
}

/**
 * Perform a Game of Life step
 */
void life_step()
{
    uint8_t i, j;
    int8_t x, y, k, l;
    uint8_t live, dead;
    uint8_t world0[HEIGHT];
    
    // Show current world
    display_load_sprite(world);
    display_advance_buffer();
    
    _delay_ms(100);
    
    // Copy old world
    memcpy(world0, world, HEIGHT);
    
    for (j = 0; j < HEIGHT; j++) {
        for (i = 0; i < WIDTH; i++) {
            live = 0;
            dead = 0;
            
            // Check neighbours
            for (k = -1; k <= 1; k++) {
                for (l = -1; l <= 1; l++) {
                    if (k == 0 && l == 0) continue;
                    y = (j + k);
                    x = (i + l);
                    
                    if (x < 0)
                        x += WIDTH;
                    if (y < 0)
                        y += HEIGHT;
                    if (x >= WIDTH)
                        x -= WIDTH;
                    if (y >= HEIGHT)
                        y -= HEIGHT;
                    
                    if (world0[y] & (1 << x)) {
                        live++;
                    } else {
                        dead++;
                    }
                }
            }
            
            if (world0[j] & (1 << i)) {
                // Alive cell
                if (live < 2 || live > 3) {
                    world[j] &= ~(1 << i);
                }
            } else {
                // Dead cell
                if (live == 3) {
                    world[j] |= (1 << i);
                }
            }
        }
    }
    
}

