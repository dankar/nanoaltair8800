#include "support.h"
#include "altair8800/intel8080.h"
#include "altair8800/irq_controller.h"
#include "altair8800/88dcdd.h"
#include "altair8800/roms.h"
#include "altair8800/memory.h"
#include "altair8800/serial.h"
#include <stdlib.h>

static bool gen_random_filename(char *buffer)
{   
#ifdef _WIN32
    char temp_path[MAX_PATH];
    uint32_t result = GetTempPathA(MAX_PATH, temp_path);
    sprintf(buffer, "%s%02x%02x%02x%02x%02x%02x%02x%02x.bin", temp_path,
            rand() % 0xff, rand() % 0xff, rand() % 0xff, rand() % 0xff,
            rand() % 0xff, rand() % 0xff, rand() % 0xff, rand() % 0xff);
    return result != 0;
#else
    return tmpnam_r(buffer) != NULL;
#endif
}

bool file_open_temporary(file_t *temp, const char *filename, uint8_t mode)
{
    file_t orig = {0};
    char filename_buffer[128];
    
    if(!file_open(&orig, filename, FILE_FLAG_READ | FILE_FLAG_OPEN))
    {
        printf("Failed to open original file\n");
        return false;
    }
    
    size_t size = file_size(&orig);
    uint8_t *data = malloc(size);
    
    bool result = file_read_buf(&orig, data, size);
    file_close(&orig);
    if(!result)
    {
        printf("Failed to read original file\n");
        free(data);
        return false;
    }
    
    for(int i = 0; i < 10; i++)
    {
        gen_random_filename(filename_buffer);
        result = file_open(temp, filename_buffer, FILE_FLAG_READ | FILE_FLAG_WRITE | FILE_FLAG_CREATE);
        if(result)
            break;
    }
    
    if(!result)
    {
        printf("Failed to open temporary file\n");
        free(data);
        return false;
    }
    
    result = file_write_buf(temp, data, size);
    file_close(temp);
    free(data);
    if(!result)
    {
        printf("Failed to write temp file\n");
        free(data);
        return false;
    }
    
    return file_open(temp, filename_buffer, mode);
}

void char_out(uint8_t b);
bool char_in(uint8_t *b);
bool sense_sw(uint8_t *b);

void emu_reset(intel8080_t *cpu, bool add_default_ports, const rom_t *rom)
{
    i8080_reset(cpu);
    irq_controller_out(0x00);
    
    if(rom)
        memory_load_rom(rom);
    
    if(add_default_ports)
    {
        port_io_t port;
        
        port.in = serial1_status;
        port.out = serial1_control;
        i8080_assign_port(cpu, 0x0, &port);
        i8080_assign_port(cpu, 0x10, &port);
        port.in = serial1_in;
        port.out = char_out;
        i8080_assign_port(cpu, 0x1, &port);
        i8080_assign_port(cpu, 0x11, &port);
        
        port.in = serial2_status;
        port.out = serial2_control;
        i8080_assign_port(cpu, 0x12, &port);
        port.in = serial2_in;
        port.out = serial2_out;
        i8080_assign_port(cpu, 0x13, &port);
        
        port.in = floppy_disk_status;
        port.out = floppy_disk_select;
        i8080_assign_port(cpu, 0x8, &port);
        port.in = floppy_sector;
        port.out = floppy_disk_function;
        i8080_assign_port(cpu, 0x9, &port);
        port.in = floppy_read;
        port.out = floppy_write;
        i8080_assign_port(cpu, 0xA, &port);
        
        
        port.out = irq_controller_out;
        port.in = 0;
        i8080_assign_port(cpu, 0xfe, &port);
        
        port.in = sense_sw;
        port.out = 0;
        i8080_assign_port(cpu, 0xff, &port);
        
    }
}
