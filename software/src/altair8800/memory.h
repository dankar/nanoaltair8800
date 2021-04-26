#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdint.h>
#include <stdbool.h>
#include "roms.h"

#define MEMORY_SIZE (1024*64)
extern uint8_t memory[MEMORY_SIZE];
extern uint8_t _write_prot[MEMORY_SIZE >> 8];

static inline uint8_t read8(uint16_t address)
{
    return memory[address];
}

static inline void write8(uint16_t address, uint8_t val)
{
    if(_write_prot[address >> 8])
    {
        return;
    }
    memory[address] = val;
}

static inline uint16_t read16(uint16_t address)
{
    uint16_t result = 0;
    result = memory[address];
    result |= read8(address+1) << 8;
    return result;
}

static inline void write16(uint16_t address, uint16_t val)
{
    if(_write_prot[address >> 8])
    {
        return;
    }
    memory[address] = val & 0xff;
    memory[address+1] = (val >> 8) & 0xff;
}

void memory_load_rom(const rom_t *rom);
void memory_disable_rom();

#endif
