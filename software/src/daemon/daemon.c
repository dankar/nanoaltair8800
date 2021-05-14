
#include <stdio.h>
#include <czmq.h>
#include <zmsg.h>
#include <zsock.h>
#include <unistd.h>
#include <zstr.h>
#include "altair8800/intel8080.h"
#include "altair8800/irq_controller.h"
#include "altair8800/88dcdd.h"
#include "altair8800/roms.h"
#include "altair8800/memory.h"
#include "altair8800/serial.h"
#include "daemon/ipc.h"
#include "sys/buffer.h"
#include "rom_def.h"
#include "support.h"
#include <signal.h>

intel8080_t cpu;
volatile bool done = false;

buffer_t terminal_buffer = {0};

void char_out(uint8_t b)
{
    if (b != 0)
    {
        buffer_push(&terminal_buffer, b);
    }
}

uint8_t sense_switches = 0x00;

bool sense_sw(uint8_t *b)
{
    *b = sense_switches;
    return true;
}

void send_term_out(ipc_t *ipc_ctx)
{
    uint8_t c;
    size_t chars = 0;
    char buffer[256];
    while(buffer_pop(&terminal_buffer, &c))
    {
        buffer[chars++] = c;
        
    }
    if(chars)
    {
        ipc_write_output(ipc_ctx, buffer, chars);
    }
}

int main(int argc, char *argv[])
{
	ipc_t ipc_ctx;

    if(ipc_init(&ipc_ctx) != IPC_OK)
    {
        fprintf(stderr, "Failed to open IPC\n");
        return 1;
    }

    usleep(1000*1000);

    if(!file_open_temporary(&floppy_disk_drive.disks[0].fp, "test/test_data/cpm63k.dsk", FILE_FLAG_READ | FILE_FLAG_OPEN | FILE_FLAG_WRITE))
    {
        fprintf(stderr, "Could not open disk 0\n");
        return 1;
    }
    if(!file_open_temporary(&floppy_disk_drive.disks[1].fp, "test/test_data/cpu_tests.dsk", FILE_FLAG_READ | FILE_FLAG_OPEN | FILE_FLAG_WRITE))
    {
        fprintf(stderr, "Could not open disk 1\n");
        return 1;
    }

    emu_reset(&cpu, true, &DBL);
    sense_switches = 0xff;
    i8080_examine(&cpu, 0xff00);
    serial1_clear_buffer();

    while(!done)
    {
        for(int i = 0; i < 1000; i++)
        {
            i8080_cycle(&cpu);
        }
        send_term_out(&ipc_ctx);
        ipc_get_input(&ipc_ctx);
        usleep(1);
    }
    
	ipc_close(&ipc_ctx);
	
	return 0;
}
