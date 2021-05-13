#ifndef _ROM_DEF_H_
#define _ROM_DEF_H_

#include <stdint.h>
#include "altair8800/roms.h"

extern const uint8_t TURMON_DATA[];
extern const uint8_t TURMONH_DATA[];
extern const uint8_t MBL_DATA[];
extern const uint8_t DBL_DATA[];

extern const rom_t TURMON;
// H replaces DUMP functionality with Intel HEX load
extern const rom_t TURMONH;
extern const rom_t MBL;
extern const rom_t DBL;


#endif
