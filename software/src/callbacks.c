
#include "sys/spi.h"
#include "sys/log.h"
#include "sys/profiler.h"
#include "altair8800/intel8080.h"
#include "altair8800/irq_controller.h"
#include "sys/usb.h"
#include "main.h"

LOG_UNIT("callbacks");

extern intel8080_t cpu;
uint32_t previous_write = 0;
volatile uint16_t address = 0;
volatile uint8_t data = 0;
volatile uint8_t status = 0;

void NO_PROFILE HAL_SYSTICK_Callback(void)
{
    address |= cpu.address_bus;
    data |= cpu.data_bus;
    status |= cpu.cached_status;
}