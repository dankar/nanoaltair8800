#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include <stdbool.h>
#include "altair8800/intel8080.h"

bool config_read(const char *filename, intel8080_t *cpu);

#endif