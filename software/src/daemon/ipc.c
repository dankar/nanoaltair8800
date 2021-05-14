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
	ctx->data_out = zsock_new_pub(OUT_DATA_URI);

	ctx->data_in = zsock_new_sub(IN_DATA_URI, "DATA");
	ctx->switches_in = zsock_new_sub(IN_DATA_URI, "SWITCH");
	ctx->control_channel = zsock_new_sub(IN_DATA_URI, "CONTROL");

	if(!ctx->data_in || !ctx->data_out || !ctx->switches_in ||
		!ctx->control_channel)
	{
		ipc_close(ctx);
		return IPC_ERROR;
	}

	return IPC_OK;
}

static zframe_t *ipc_get_msg_content(zsock_t *sock)
{
	zmsg_t *msg = zmsg_recv_nowait(sock);
	if(msg)
	{
		zframe_t *type = zmsg_pop(msg);
		zframe_t *content = zmsg_pop(msg);
		zframe_destroy(&type);
		zmsg_destroy(&msg);
		return content;
	}
	return NULL;
}
	

// Receive stuff
ipc_result_t ipc_set_lights(ipc_t *ctx, uint16_t address, uint8_t data);
ipc_result_t ipc_get_switches(ipc_t *ctx, uint16_t *switches);

ipc_result_t ipc_get_input(ipc_t *ctx)
{
	bool data;
	do
	{
		data = false;
		zframe_t *content = ipc_get_msg_content(ctx->data_in);

		if(content)
		{
			data = true;
			char *str = zframe_strdup(content);
			serial1_add_data((uint8_t*)str, zframe_size(content));
			
			free(str);
			zframe_destroy(&content);
		}
	}while(data);

	return IPC_OK;
}

bool ipc_is_reset_pending(ipc_t *ctx)
{
	bool result = false;
	zframe_t *content = ipc_get_msg_content(ctx->control_channel);

	if(content)
	{
		char *str = zframe_strdup(content);
		printf("CONTROL: %s\n", str);
		zframe_destroy(&content);
		if(strcmp(str, "RESET") == 0)
		{
			result = true;
		}
		free(str);
	}
	return result;
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
	zsock_destroy(&ctx->control_channel);
}
