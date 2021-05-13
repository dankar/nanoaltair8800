#ifndef _SUPPORT_H_
#define _SUPPORT_H_

#include "sys/filesystem.h"
#include "altair8800/roms.h"
#include "altair8800/intel8080.h"

bool file_open_temporary(file_t *temp, const char *filename, uint8_t mode);
void emu_reset(intel8080_t *cpu, bool add_default_ports, const rom_t *rom);

#endif
