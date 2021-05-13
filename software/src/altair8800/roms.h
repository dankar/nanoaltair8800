#ifndef _ROMS_H_
#define _ROMS_H_

#include <stdint.h>

typedef struct rom_t
{
    const uint16_t offset;
    const uint16_t size;
    const uint8_t *data;
} rom_t;

#endif
