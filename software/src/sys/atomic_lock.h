#ifndef _ATOMIC_LOCK_H_
#define _ATOMIC_LOCK_H_

#include <stdint.h>
#include <stdbool.h>
#include "cmsis_gcc.h"
#include "sys/profiler.h"

typedef volatile uint8_t lock_t;

static inline NO_PROFILE bool lock(lock_t *semaphore)
{
    bool result = true;
    do
    {
        if(__LDREXB(semaphore))
        {
            result = false;
            break;
        }
    } while(__STREXB(1, semaphore));
    __DMB();
    return result;
}

static inline NO_PROFILE void unlock(lock_t *semaphore)
{
    *semaphore = 0;
}

#endif