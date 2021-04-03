#include "irq_controller.h"
#include "sys/log.h"

LOG_UNIT("irq_controller");
LOG_TYPE(IRQ_DEBUG, OUTPUT_ON);

uint8_t irq_status = 0;
intel8080_t *cpu_ptr = NULL;
uint64_t rtc_time_passed_micros = 0;
uint8_t rtc_divider = 1;
uint8_t rtc_interrupt_line = 0xff;

void irq_controller_out(uint8_t b)
{
    irq_status = b;
}

void irq_rtc_set_divider(uint8_t div)
{
    rtc_divider = div;
}

void irq_rtc_set_line(uint8_t line)
{
    rtc_interrupt_line = line;
}