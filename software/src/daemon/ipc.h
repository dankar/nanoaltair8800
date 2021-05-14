#ifndef _IPC_H_
#define _IPC_H_

#include <stdbool.h>
#include <stdint.h>
#include <czmq.h>
#include <zmsg.h>
#include <zsock.h>
#include <unistd.h>
#include <zstr.h>
#include "sys/buffer.h"

#define OUT_DATA_URI "ipc://@/nanoaltair8800.termout"
#define IN_DATA_URI "ipc://@/nanoaltair8800.termin"

// Out data spec
//
// DATA - Terminal chars out from emulator
//   - string - One or more terminal chars
// LIGHTS - Front panel lights
//   - uint16_t - address lights
//   - uint8_t - data lights
//
// In data spec
// DATA - Terminal chars in to emulator
//   - string - One or more terminal chars

typedef enum ipc_result_t
{
	IPC_ERROR = -1,
	IPC_OK = 0,
	IPC_NO_DATA = 1
} ipc_result_t;

typedef struct ipc_t
{
	zsock_t *data_out;
	zsock_t *data_in;
	zsock_t *lights_out;
	zsock_t *switches_in;
	zsock_t *control_channel;
} ipc_t;

ipc_result_t ipc_init(ipc_t *ctx);

// Receive stuff
ipc_result_t ipc_set_lights(ipc_t *ctx, uint16_t address, uint8_t data);
ipc_result_t ipc_get_switches(ipc_t *ctx, uint16_t *switches);
ipc_result_t ipc_get_input(ipc_t *ctx);

bool ipc_is_reset_pending(ipc_t *ctx);

// Send stuff
ipc_result_t ipc_write_output(ipc_t *ctx, void *data, size_t len);
void ipc_close(ipc_t *ctx);

#endif
