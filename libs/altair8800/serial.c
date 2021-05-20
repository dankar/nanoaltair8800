#include "serial.h"
#include "sys/usb.h"
#include "sys/log.h"
#include "sys/buffer.h"
#include "altair8800/intel8080.h"
#include "altair8800/irq_controller.h"
#include <string.h>

LOG_UNIT("serial");
LOG_TYPE(SERIAL_LOAD, OUTPUT_OFF);

buffer_t serial1_rx_buffer = {0};
uint8_t serial1_interrupt_line = 0xff;
bool serial1_rx_interrupt_enabled = false;
bool serial1_tx_interrupt_enabled = false;
extern intel8080_t cpu;

void serial1_set_interrupt_line(uint8_t line)
{
    serial1_interrupt_line = line;
}

void serial1_out(uint8_t b)
{
	//b = b & 0x7f;
#ifndef TEST_BUILD
    uint32_t start = HAL_GetTick();
	while(!usb_transmit(&b, 1) && (HAL_GetTick() - start) < 10);
#endif
}

bool serial1_in(uint8_t *b)
{
    if(buffer_pop(&serial1_rx_buffer, b))
    {
        return true;
    }
    return false;
}

void serial1_clear_buffer()
{
    serial1_rx_buffer.head = serial1_rx_buffer.tail = 0;
}

bool serial1_status(uint8_t *b)
{
    *b = SIO_STATUS_TDRE;
    
    if(!buffer_is_empty(&serial1_rx_buffer))
    {
        *b |= SIO_STATUS_RDRF;
        if(serial1_rx_interrupt_enabled)
        {
            *b |= SIO_STATUS_INTERRUPT;
        }
    }
    
    return true;
}

void serial1_control(uint8_t b)
{
    if(b & SIO_RX_INTERRUPT)
    {
        serial1_rx_interrupt_enabled = true;
    }
    else
    {
        serial1_rx_interrupt_enabled = false;
    }
    
    if(b & SIO_TX_INTERRUPT && !(b & SIO_TX_INTERRUPT2))
    {
        serial1_tx_interrupt_enabled = true;
    }
    else
    {
        serial1_tx_interrupt_enabled = false;
    }
}

void serial1_add_data(const uint8_t *data, uint32_t len)
{
    if(len > 0 && serial1_rx_interrupt_enabled)
    {
        irq_controller_irq(serial1_interrupt_line);
    }
    for(uint32_t i = 0; i < len; i++)
    {
        if(!buffer_push(&serial1_rx_buffer, data[i]))
            return;
    }
}

file_t serial2_tape_file = {0};

void serial2_out(uint8_t b)
{
	
}

bool serial2_in(uint8_t *b)
{
    if(!file_is_valid(&serial2_tape_file))
    {
        return false;
    }
    
    bool res = file_read(&serial2_tape_file, b);
    
    if(file_eof(&serial2_tape_file))
    {
        file_close(&serial2_tape_file);
        LOG(SERIAL_LOAD, "Load from tape complete\n");
    }
    return res;
}

bool serial2_status(uint8_t *b)
{
    *b = SIO_STATUS_TDRE;
    if(file_is_valid(&serial2_tape_file) && !file_eof(&serial2_tape_file))
    {
        *b |= SIO_STATUS_RDRF;
    }
    return true;
}

void serial2_control(uint8_t b)
{
    
}

bool serial2_attach_tape(const char *filename)
{
    return file_open(&serial2_tape_file, filename, FILE_FLAG_READ | FILE_FLAG_OPEN);
}