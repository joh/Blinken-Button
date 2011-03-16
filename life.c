#include <string.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#include "display.h"
#include "random.h"
#include "life.h"

#define WIDTH  8
#define HEIGHT 8

/* Our binary world */
uint8_t world[HEIGHT] = 
{ 
  0b00000000,
  0b00001010,
  0b00010000,
  0b00010001,
  0b00011110,
  0b00000000,
  0b00000000,
  0b00000000
};

/* World timeout counter */
uint8_t timeout_counter = 0;

/**
 * Randomize world
 */
void life_randomize()
{
    uint8_t i;
    
    // Random world
    for (i = 0; i < HEIGHT; i++) {
        world[i] = get_random(0xff);
    }
}

/**
 * Initialize Game of Life
 */
void life_init()
{
    // Randomize world
    randomize_seed();
    life_randomize();
    
    // Set up timer
    power_timer1_enable();
    
    TCCR1A = 0;
    TCCR1B = _BV(CS12) | _BV(WGM12);    // Prescaler 256, CTC mode
    TIMSK1 = _BV(OCIE1A);               // Interrupt on Output Compare Match A
    //OCR1A = 31249;                      // 1Hz @ 8MHz
    OCR1A = 15624;                      // 2Hz @ 8MHz
    //OCR1A = 3124;                       // 10Hz @ 8MHz
}

/**
 * Timer1 interrupt service routine
 */
ISR (TIMER1_COMPA_vect)
{
    // Allow display interrupts
    sei();
    
    // Perform a life step
    life_step();
}

/**
 * Count the number of (alive) neighbour cells at row j, col i
 */
static uint8_t neighbours(uint8_t *world, uint8_t j, uint8_t i)
{
    int8_t k, l, x, y;
    uint8_t count = 0;
    
    for (k = -1; k <= 1; k++) {
        for (l = -1; l <= 1; l++) {
            if (k == 0 && l == 0) continue;
            y = (j + k);
            x = (i + l);
            
            // Boundary
            if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) continue;
            
            // Wrap around
            /*
            if (x < 0)
                x += WIDTH;
            if (y < 0)
                y += HEIGHT;
            if (x >= WIDTH)
                x -= WIDTH;
            if (y >= HEIGHT)
                y -= HEIGHT;
            */
            
            if (world[y] & (1 << x)) {
                count++;
            }
        }
    }
    
    return count;
}

/**
 * Perform a Game of Life step
 */
void life_step()
{
    uint8_t i, j, live;
    uint8_t world0[HEIGHT];
    
    // Show current world
    display_load_sprite(world);
    display_advance_buffer();
    
    // If world is static / empty, randomize
    if (timeout_counter > LIFE_TIMEOUT) {
        timeout_counter = 0;
        life_randomize();
        return;
    }
    
    if (timeout_counter > 0) {
        timeout_counter++;
        return;
    }
    
    // Copy old world
    memcpy(world0, world, HEIGHT);
    
    // Game of Life step
    for (j = 0; j < HEIGHT; j++) {
        for (i = 0; i < WIDTH; i++) {
            // Check neighbours
            live = neighbours(world0, j, i);
            
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
    
    // Detect static / empty configurations
    timeout_counter = 1;
    for (j = 0; j < HEIGHT; j++) {
        if (world0[j] != world[j]) {
            // Not static
            timeout_counter = 0;
            break;
        }
    }
}

