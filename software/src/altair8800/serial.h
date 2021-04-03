#ifndef _SERIAL_H_
#define _SERIAL_H_

#define SIO_CLOCK_1	    0x0
#define SIO_CLOCK_16    0x1
#define SIO_CLOCK_64    0x2
#define SIO_RESET       0x3
#define SIO_CLOCK_MASK  0x3

#define SIO_TX_INTERRUPT	(1<<5)
#define SIO_TX_INTERRUPT2   (1<<6)
#define SIO_RX_INTERRUPT	(1<<7)

#define SIO_STATUS_RDRF		(1<<0) // Receive data full
#define SIO_STATUS_TDRE		(1<<1) // Transmit data empty
#define SIO_STATUS_INTERRUPT (1<<7)

#include <stdint.h>
#include <stdbool.h>
#include "sys/filesystem.h"

void serial1_clear_buffer();
void serial1_set_interrupt_line(uint8_t line);
void serial1_out(uint8_t b);
bool serial1_in(uint8_t *b);
bool serial1_status(uint8_t *b);
void serial1_control(uint8_t b);
void serial1_add_data(const uint8_t *data, uint32_t len);
void serial2_out(uint8_t b);
bool serial2_in(uint8_t *b);
bool serial2_status(uint8_t *b);
void serial2_control(uint8_t b);

extern file_t serial2_tape_file;

bool serial2_attach_tape(const char *filename);

#endif