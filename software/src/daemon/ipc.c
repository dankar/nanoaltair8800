#include "ipc.h"
#include <zframe.h>
#include <zmsg.h>
#include <zsock.h>
#include <string.h>
#include "altair8800/serial.h"
#include "sys/buffer.h"

ipc_result_t ipc_init(ipc_t *ctx)
{
	memset(ctx, 0, sizeof(ipc_t));
	ctx->data_in = zsock_new_sub(IN_DATA_URI, "DATA");
	ctx->data_out = zsock_new_pub(OUT_DATA_URI);
	ctx->switches_in = zsock_new_sub(IN_DATA_URI, "SWITCH");

	if(!ctx->data_in || !ctx->data_out || !ctx->switches_in)
	{
		ipc_close(ctx);
		return IPC_ERROR;
	}

	return IPC_OK;
}

// Receive stuff
ipc_result_t ipc_set_lights(ipc_t *ctx, uint16_t address, uint8_t data);
ipc_result_t ipc_get_switches(ipc_t *ctx, uint16_t *switches);

ipc_result_t ipc_get_input(ipc_t *ctx)
{
	zmsg_t *msg;

	bool data;
	do
	{
		data = false;
		msg = zmsg_recv_nowait(ctx->data_in);

		if(msg)
		{
			data = true;
			zframe_t *type = zmsg_pop(msg);
			zframe_t *content = zmsg_pop(msg);
			
			char *str = zframe_strdup(content);
			serial1_add_data((uint8_t*)str, zframe_size(content));
			
			free(str);
			zframe_destroy(&type);
			zframe_destroy(&content);
			zmsg_destroy(&msg);
		}
	}while(data);

	return IPC_OK;
}

// Send stuff
ipc_result_t ipc_write_output(ipc_t *ctx, void *data, size_t len)
{
	zmsg_t *msg = zmsg_new();
	zmsg_addstr(msg, "DATA");
	zmsg_addmem(msg, data, len);
	zmsg_send(&msg, ctx->data_out);
	return IPC_OK;
}

void ipc_close(ipc_t *ctx)
{
	zsock_destroy(&ctx->data_in);
	zsock_destroy(&ctx->data_out);
	zsock_destroy(&ctx->switches_in);
}
