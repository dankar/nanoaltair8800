#ifndef _IRQ_CONTROLLER_H_
#define _IRQ_CONTROLLER_H_

#include <stdint.h>
#include <stdbool.h>
#include "sys/log.h"
#include "altair8800/intel8080.h"

#define IRQ_CURRENT_LEVEL(x)        (x & 0b111)
#define IRQ_DISABLE_CURRENT_LEVEL   (1<<3)
#define IRQ_RESET_RTC_INTERRUPT     (1<<4)
#define IRQ_CLEAR_RTC_COUNTER       (1<<5)
#define IRQ_ENABLE_RTC_INTERRUPT    (1<<6)
#define IRQ_ENABLE_IRQ_CONTROLLER   (1<<7)
#define NUM_INTERRUPT_LINES 8

extern uint8_t irq_status;
extern intel8080_t cpu;
extern uint64_t rtc_time_passed_micros;
extern uint8_t rtc_divider;
extern uint8_t rtc_interrupt_line;

void irq_controller_out(uint8_t b);
void irq_rtc_set_divider(uint8_t div);
void irq_rtc_set_line(uint8_t line);

static inline void irq_controller_irq(uint8_t line)
{
    if(line >= NUM_INTERRUPT_LINES)
    {
        return;
    }
    
    if(irq_status & IRQ_ENABLE_IRQ_CONTROLLER)
    {
        if(line == rtc_interrupt_line && !(irq_status & IRQ_ENABLE_RTC_INTERRUPT))
        {
            return;
        }
        // If level checking is disabled or the requested interrupt is lower than the current
        // allowed level, fire the interrupt
        if(!(irq_status & IRQ_DISABLE_CURRENT_LEVEL) || line < IRQ_CURRENT_LEVEL(irq_status))
        {
            i8080_interrupt(&cpu, line);
        }
    }
}

static inline void irq_rtc_fire()
{
    irq_controller_irq(rtc_interrupt_line);
}

#endif