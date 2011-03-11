#include <string.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "display.h"
#include "random.h"
#include "life.h"

#define WIDTH  8
#define HEIGHT 8

uint8_t world[HEIGHT] = 
{ 0b00000000,
  0b00000000,
  0b00010100,
  0b00100000,
  0b00100010,
  0b00111100,
  0b00000000,
  0b00000000
};

/**
 * Initialize Game of Life
 */
void life_init()
{
    uint8_t i;
    
    // Random world
    randomize_seed();
    for (i = 0; i < HEIGHT; i++) {
        world[i] = get_random(0xff);
    }
    
    // Set up timer
    power_timer1_enable();
    
    TCCR1A = 0;
    TCCR1B = _BV(CS12) | _BV(WGM12);    // Prescaler 256, CTC mode
    TIMSK1 = _BV(OCIE1A);               // Interrupt on Output Compare Match A
    //OCR1A = 31249;                      // 1Hz @ 8MHz
    OCR1A = 15624;                      // 2Hz @ 8MHz
    //OCR1A = 3124;                       // 10Hz @ 8MHz
}

ISR (TIMER1_COMPA_vect)
{
    life_step();
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
    
    //_delay_ms(100);
    
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

