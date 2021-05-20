#ifndef _SPI_H_
#define _SPI_H_

#include <stdint.h>
#include <stdbool.h>
#include "main.h"

extern uint8_t led_data[4] __attribute__((aligned (4)));

bool SPI_write_leds(uint16_t address, uint8_t data, uint8_t status);

#endif