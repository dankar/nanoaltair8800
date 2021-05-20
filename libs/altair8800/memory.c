#include "memory.h"
#include <string.h>

uint8_t memory[MEMORY_SIZE];
uint8_t _write_prot[0x100] = {0};

void memory_load_rom(const rom_t *rom)
{
    memcpy(&memory[rom->offset], rom->data, rom->size);
    for(int j = 0; j < rom->size / 0x100; j++)
    {
        _write_prot[(rom->offset >> 8) + j] = 1;
    }
}

void memory_disable_rom()
{
    memset(_write_prot, 0, sizeof(_write_prot));
}