#ifndef LIFE_H_
#define LIFE_H_

// Reset timeout for static / empty worlds
#define LIFE_TIMEOUT 10

// Possible world modes
#define LIFE_WORLD_BOUNDED 0x01     // Bounded world
#define LIFE_WORLD_WRAP    0x02     // Wrap around

// World mode
#define LIFE_WORLD_MODE    LIFE_WORLD_BOUNDED

// Functions
void life_init(void);
void life_step(void);

#endif /* LIFE_H_ */
