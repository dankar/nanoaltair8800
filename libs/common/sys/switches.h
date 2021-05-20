#ifndef _SWITCHES_H_
#define _SWITCHES_H_

#include <stdbool.h>

// The switches are debounced by first triggering on the first
// falling edge, and then not allowing retriggering until
// SWITCH_DEBOUNCE_MS has passed. 10ms seems like a good number.
#define SWITCH_DEBOUNCE_MS 10

typedef enum
{
    SWITCH_STOP = 0,
    SWITCH_RUN,
    SWITCH_STEP,
    SWITCH_RESET,
    SWITCH_EXAMINE,
    SWITCH_EXAMINE_NEXT,
    SWITCH_DEPOSIT,
    SWITCH_DEPOSIT_NEXT,
    SWITCH_NUM_SWITCHES
} switches_t;

typedef enum
{
    SWITCH_STATE_UP = 0,
    SWITCH_STATE_DOWN = 1
} switch_state_t;

bool switch_get(switches_t sw);
bool switch_was_pressed(switches_t sw);
uint16_t switch_get_address();
bool sense(uint8_t *b);
void switch_clear_all();

#endif