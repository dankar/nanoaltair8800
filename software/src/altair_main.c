#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "sys/log.h"
#include "sys/usb.h"
#include "sys/switches.h"
#include "sys/spi.h"
#include "sys/timer.h"
#include "sys/hw_timer.h"
#include "sys/profiler.h"
#include "fatfs.h"
#include "altair8800/intel8080.h"
#include "altair8800/88dcdd.h"
#include "altair8800/memory.h"
#include "altair8800/serial.h"
#include "altair8800/irq_controller.h"
#include "config/config.h"

LOG_UNIT("main");
LOG_TYPE(MAIN_DEBUG, OUTPUT_ON);

uint16_t bus_switches = 0;

intel8080_t cpu;

extern volatile uint16_t address;
extern volatile uint8_t data;
extern volatile uint8_t status;

// Read configuration and flash LEDs in a pattern if the configuration or SD card
// is unavailable.
void read_cfg(intel8080_t *cpu)
{
    const char *default_cfg = "altair.cfg";

    if(!config_read(default_cfg, cpu))
    {
        for(int i = 0; i < 5; i++)
        {
            SPI_write_leds(0xaaaa, 0xaa, 0xaa);
            HAL_Delay(100);
            SPI_write_leds(0x5555, 0x55, 0x55);
            HAL_Delay(100);
        }
        SPI_write_leds(0, 0, 0);
    }
}

void print_version_info()
{
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\r\n");
    printf("nanoaltair, build " BUILD_VERSION "\r\n");
    printf("Build timestamp: " __DATE__ " " __TIME__ "\r\n");
    printf("Copyright Daniel Karling, 2021\r\n");
}

// Tight loop for running the emulator, with occasional updates of the
// LEDs and check for stop switch presses.
//
void run_sim()
{
    timer_t led_timer = 0;
    
#if PROFILER_ENABLED == 1
    timer_t cycle_timer = 0;
    uint32_t cycle_counter = 0;
    timer_periodic(&cycle_timer, 5000);
#endif
    timer_periodic(&led_timer, 10);
    hw_timer_start_tim1();

    while(true)
    {
        for(int i = 0; i < 1000; i++)
        {
#if PROFILER_ENABLED == 1
            cycle_counter++;
#endif
            i8080_cycle(&cpu);
        }
        
        if(timer_periodic(&led_timer, 10))
        {
            if(switch_was_pressed(SWITCH_STOP))
                break;
            if(address || data || status)
                SPI_write_leds(address, data, status);
            address = data = status = 0;
        }
#if PROFILER_ENABLED == 1
        
        if(timer_periodic(&cycle_timer, 10000))
        {
            profiler_print();
            profiler_reset();
            printf("%ld cycles executed\r\n", cycle_counter);
            cycle_counter = 0;
        }
#endif
    }
    
    hw_timer_stop_tim1();
    switch_clear_all();
    cpu.run_state = false;
}

// Handle the stopped mode. Check all other key presses, including
// the special presses for entering the bootloader and printing firmware
// version
void handle_stopped()
{
    while(!switch_was_pressed(SWITCH_RUN))
    {
        if(address || data || status)
            SPI_write_leds(address, data, status);
        address = data = status = 0;
        
        if(switch_was_pressed(SWITCH_RESET))
        {
            if(switch_get(SWITCH_STOP))
            {
                CallBootloader();
            }
            read_cfg(&cpu);
            SPI_write_leds(cpu.address_bus, cpu.data_bus, cpu.registers.status_byte);
            address = data = status = 0;
        }
        if(switch_was_pressed(SWITCH_STEP))
        {
            i8080_cycle(&cpu);
        }
        if(switch_was_pressed(SWITCH_EXAMINE))
        {
            if(switch_get(SWITCH_STOP))
            {
                print_version_info();
            }
            else
            {
                i8080_examine(&cpu, switch_get_address());
            }
        }
        if(switch_was_pressed(SWITCH_EXAMINE_NEXT))
        {
            i8080_examine_next(&cpu);
        }
        if(switch_was_pressed(SWITCH_DEPOSIT))
        {
            i8080_deposit(&cpu, switch_get_address() & 0xff);
        }
        if(switch_was_pressed(SWITCH_DEPOSIT_NEXT))
        {
            i8080_deposit_next(&cpu, switch_get_address() & 0xff);
        }
    }
    switch_clear_all();
    cpu.run_state = true;
}

// The main function
void altair_main()
{
    LOG(MAIN_DEBUG, "Main start\n");
    
    SPI_write_leds(0x0000, 0x00, 0x00); 
    read_cfg(&cpu);
    
    for(;;)
    {
        if(cpu.run_state)
        {
            run_sim();
        }
        else
        {
            handle_stopped();
        }
    }
}
