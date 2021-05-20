#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdint.h>
#include <stdbool.h>
#include "main.h"

typedef uint32_t timer_t;

static inline void timer_set(timer_t *t, uint32_t period)
{
    *t = HAL_GetTick() + period;
}

static inline bool timer_periodic(timer_t *t, uint32_t period)
{
    uint32_t tick = HAL_GetTick();
    if(*t < tick)
    {
        *t = tick + period;
        return true;
    }
    return false;
}

static inline bool timer_expired(timer_t *t)
{
    return HAL_GetTick() > *t;
}

#endif