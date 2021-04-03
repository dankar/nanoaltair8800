#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "altair8800/intel8080.h"
#include "altair8800/irq_controller.h"
#include "altair8800/88dcdd.h"
#include "altair8800/roms.h"
#include "altair8800/memory.h"
#include "altair8800/serial.h"
#include "testing.h"
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif

intel8080_t cpu;

#define BUFFER_SIZE 4096
char buffer[BUFFER_SIZE];
uint32_t buffer_begin = BUFFER_SIZE;
uint8_t new_data = 0;

void add_to_buffer(uint8_t b)
{
    char temp[BUFFER_SIZE];
    memcpy(temp, &buffer[1], BUFFER_SIZE-1);
    temp[BUFFER_SIZE-1] = b;
    memcpy(buffer, temp, BUFFER_SIZE);
    
    new_data = 1;
    
    if(buffer_begin)
    {
        buffer_begin--;
    }
}

bool match_output(const char *str, const char *err, uint8_t *error_out)
{
    if(new_data)
    {
        new_data = 0;
        if(err && error_out)
        {
            *error_out = memcmp(&buffer[BUFFER_SIZE-strlen(err)], err, strlen(str)) == 0;
        }
        return memcmp(&buffer[BUFFER_SIZE-strlen(str)], str, strlen(str)) == 0;
    }
    else
        return false;
}

void print_buffer()
{
    char temp[BUFFER_SIZE+1];
    memcpy(temp, buffer, BUFFER_SIZE);
    temp[BUFFER_SIZE] = '\0';
    printf("\"%s\"\n", &temp[buffer_begin]);
}

void test_print_error()
{
    print_buffer();
}

void char_out(uint8_t b)
{
    if (b != 0)
    {
        add_to_buffer(b);
    }
}

bool char_in(uint8_t *b)
{
    return false;
}

uint8_t sense_switches = 0x00;

bool sense_sw(uint8_t *b)
{
    *b = sense_switches;
    return true;
}

bool gen_random_filename(char *buffer)
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

const uint8_t TURMON_DATA[] = {0x3e, 0x03, 0xd3, 0x10, 0x3e, 0x11, 0xd3, 0x10, 0x31, 0x00, 0xf8, 0xcd, 0x9d, 0xfd, 0x3e, 0x2e, 0xcd, 0xf2, 0xfd, 0xcd, 0xe8, 0xfd, 0xfe, 0x4d, 0xca, 0x29, 0xfd, 0xfe, 0x44, 0xcc, 0x4f, 0xfd, 0xfe, 0x4a, 0xc2, 0x08, 0xfd, 0xcd, 0xa7, 0xfd, 0xe9, 0xcd, 0xa7, 0xfd, 0x3e, 0x23, 0xcd, 0x9d, 0xfd, 0x54, 0x5d, 0xcd, 0xc9, 0xfd, 0x1a, 0x67, 0xcd, 0xcf, 0xfd, 0xcd, 0xa8, 0xfd, 0xeb, 0xda, 0x2d, 0xfd, 0x77, 0xbe, 0xca, 0x2d, 0xfd, 0x3e, 0x3f, 0xcd, 0xf2, 0xfd, 0xc3, 0x08, 0xfd, 0xcd, 0xa7, 0xfd, 0xeb, 0xd4, 0xe3, 0xfd, 0xcd, 0xa7, 0xfd, 0x3e, 0x0d, 0x06, 0x3c, 0xcd, 0xf2, 0xfd, 0x05, 0xc2, 0x5d, 0xfd, 0xb8, 0x78, 0xc2, 0x5b, 0xfd, 0x7d, 0x93, 0x6f, 0x7c, 0x9a, 0x67, 0x23, 0x05, 0x7c, 0xb7, 0xc2, 0x77, 0xfd, 0x45, 0x3e, 0x3c, 0xcd, 0xf2, 0xfd, 0x78, 0xcd, 0xf2, 0xfd, 0x0e, 0x00, 0x7b, 0xcd, 0xf2, 0xfd, 0x7a, 0xcd, 0xf2, 0xfd, 0x1a, 0xcd, 0xf2, 0xfd, 0x13, 0x2b, 0x05, 0xc2, 0x8a, 0xfd, 0x79, 0xcd, 0xf2, 0xfd, 0x7c, 0xb5, 0xc2, 0x70, 0xfd, 0x3e, 0x0d, 0xcd, 0xf2, 0xfd, 0x3e, 0x0a, 0xc3, 0xf2, 0xfd, 0x06, 0x06, 0x03, 0x21, 0x00, 0x00, 0xcd, 0xe8, 0xfd, 0x4f, 0xfe, 0x20, 0x37, 0xc8, 0xe6, 0xb8, 0xee, 0x30, 0xc2, 0x47, 0xfd, 0x79, 0xe6, 0x07, 0x29, 0x29, 0x29, 0x85, 0x6f, 0x05, 0xc2, 0xad, 0xfd, 0xc9, 0x06, 0x06, 0xaf, 0xc3, 0xd6, 0xfd, 0x06, 0x03, 0xe6, 0x29, 0x17, 0x29, 0x17, 0x29, 0x17, 0xe6, 0x07, 0xf6, 0x30, 0xcd, 0xf2, 0xfd, 0x05, 0xc2, 0xd2, 0xfd, 0x3e, 0x20, 0xc3, 0xf2, 0xfd, 0xdb, 0x10, 0x0f, 0xd2, 0xe8, 0xfd, 0xdb, 0x11, 0xe6, 0x7f, 0xf5, 0x81, 0x4f, 0xdb, 0x10, 0x0f, 0x0f, 0xd2, 0xf5, 0xfd, 0xf1, 0xd3, 0x11, 0xc9};
const uint8_t TURMONH_DATA[] = {0x3e, 0x03, 0xd3, 0x10, 0x3e, 0x11, 0xd3, 0x10, 0x31, 0x00, 0x80, 0xcd, 0x9f, 0xfd, 0x3e, 0x2e, 0xcd, 0xf4, 0xfd, 0xcd, 0xec, 0xfd, 0xfe, 0x4d, 0xca, 0x29, 0xfd, 0xfe, 0x48, 0xcc, 0x4f, 0xfd, 0xfe, 0x4a, 0xc2, 0x08, 0xfd, 0xcd, 0xa9, 0xfd, 0xe9, 0xcd, 0xa9, 0xfd, 0x3e, 0x23, 0xcd, 0x9f, 0xfd, 0x54, 0x5d, 0xcd, 0xcd, 0xfd, 0x1a, 0x67, 0xcd, 0xd3, 0xfd, 0xcd, 0xaa, 0xfd, 0xeb, 0xda, 0x2d, 0xfd, 0x77, 0xbe, 0xca, 0x2d, 0xfd, 0x3e, 0x3f, 0xcd, 0xf4, 0xfd, 0xc3, 0x08, 0xfd, 0xcd, 0x8c, 0xfd, 0xc2, 0x4f, 0xfd, 0xcd, 0xf4, 0xfd, 0xcd, 0x7f, 0xfd, 0xca, 0x76, 0xfd, 0x57, 0xcd, 0x7f, 0xfd, 0x67, 0xcd, 0x7f, 0xfd, 0x6f, 0xcd, 0x7f, 0xfd, 0xcd, 0x7f, 0xfd, 0x77, 0x23, 0x15, 0xc2, 0x6a, 0xfd, 0xc3, 0x4f, 0xfd, 0xdb, 0x11, 0x1b, 0x7a, 0xb3, 0xc2, 0x76, 0xfd, 0xc9, 0xcd, 0x8c, 0xfd, 0x87, 0x87, 0x87, 0x87, 0x5f, 0xcd, 0x8c, 0xfd, 0x83, 0xc9, 0xdb, 0x10, 0x0f, 0xd2, 0x8c, 0xfd, 0xdb, 0x11, 0xfe, 0x3a, 0xc8, 0xd6, 0x30, 0xfe, 0x0a, 0xd8, 0xd6, 0x07, 0xc9, 0x3e, 0x0d, 0xcd, 0xf4, 0xfd, 0x3e, 0x0a, 0xc3, 0xf4, 0xfd, 0x06, 0x06, 0x03, 0x21, 0x00, 0x00, 0x0e, 0x01, 0xcd, 0xec, 0xfd, 0x4f, 0xfe, 0x20, 0x37, 0xc8, 0xe6, 0xb8, 0xee, 0x30, 0xc2, 0x47, 0xfd, 0x79, 0xe6, 0x07, 0x29, 0x29, 0x29, 0x85, 0x6f, 0x05, 0xc2, 0xaf, 0xfd, 0xc9, 0x06, 0x06, 0xaf, 0xc3, 0xda, 0xfd, 0x06, 0x03, 0xe6, 0x29, 0x17, 0x29, 0x17, 0x29, 0x17, 0xe6, 0x07, 0xf6, 0x30, 0xcd, 0xf4, 0xfd, 0x05, 0xc2, 0xd6, 0xfd, 0x3e, 0x20, 0xc3, 0xf4, 0xfd, 0xdb, 0x10, 0x0f, 0xd2, 0xec, 0xfd, 0xdb, 0x11, 0xf5, 0xdb, 0x10, 0x0f, 0x0f, 0xd2, 0xf5, 0xfd, 0xf1, 0xd3, 0x11, 0xc9};
const uint8_t MBL_DATA[] = {0xf3, 0x21, 0xff, 0xff, 0x23, 0x7e, 0x47, 0x2f, 0x77, 0xbe, 0x70, 0xca, 0x04, 0xfe, 0x7d, 0xb7, 0xc2, 0xbb, 0xfe, 0x16, 0xfe, 0x2b, 0x5d, 0x1a, 0x77, 0x7b, 0xfe, 0x41, 0xc2, 0x15, 0xfe, 0xf9, 0xe5, 0x1d, 0x1a, 0x6f, 0x74, 0xd6, 0x74, 0xc2, 0x21, 0xfe, 0x54, 0xc9, 0x74, 0x7a, 0x7d, 0x82, 0x85, 0x89, 0x8d, 0x91, 0x98, 0x9d, 0xa3, 0xa8, 0xad, 0xb5, 0xb8, 0xbf, 0xc4, 0xc7, 0xdc, 0xdf, 0xe7, 0xd3, 0x20, 0xd3, 0x21, 0xd3, 0x22, 0x2f, 0xd3, 0x23, 0x3e, 0x2c, 0xd3, 0x20, 0xd3, 0x22, 0x3e, 0x03, 0xd3, 0x10, 0xd3, 0x12, 0x3e, 0x11, 0xd3, 0x10, 0xd3, 0x12, 0xdb, 0xff, 0xe6, 0x07, 0x87, 0xc6, 0xf0, 0x5f, 0x1a, 0x2e, 0xe9, 0x1f, 0x77, 0x2e, 0xe2, 0x3d, 0x77, 0x1c, 0x1a, 0x2e, 0xe4, 0x77, 0xd2, 0x78, 0xfe, 0x2c, 0x36, 0xc2, 0xcd, 0xe8, 0xfe, 0xcd, 0xe1, 0xfe, 0x4f, 0xb7, 0xca, 0x92, 0xfe, 0xcd, 0xe1, 0xfe, 0xb9, 0xca, 0x83, 0xfe, 0x0d, 0xcd, 0xe1, 0xfe, 0x0d, 0xc2, 0x8b, 0xfe, 0x62, 0x2e, 0x92, 0xe5, 0xcd, 0xe1, 0xfe, 0xfe, 0x3c, 0xca, 0xa6, 0xfe, 0xfe, 0x78, 0xc0, 0xcd, 0xdd, 0xfe, 0x67, 0xe9, 0xcd, 0xe1, 0xfe, 0x41, 0x4f, 0xcd, 0xdd, 0xfe, 0x67, 0x7a, 0xbc, 0x3e, 0x4f, 0xca, 0xcb, 0xfe, 0xcd, 0xe1, 0xfe, 0x77, 0xbe, 0x3e, 0x4d, 0xc2, 0xcb, 0xfe, 0x23, 0x0d, 0xc2, 0xaf, 0xfe, 0xcd, 0xe1, 0xfe, 0xc8, 0x3e, 0x43, 0x32, 0x00, 0x00, 0x22, 0x01, 0x00, 0xfb, 0xd3, 0x01, 0xd3, 0x11, 0xd3, 0x05, 0xd3, 0x23, 0xc3, 0xd2, 0xfe, 0xcd, 0xe1, 0xfe, 0x6f, 0xdb, 0x00, 0xe6, 0x00, 0xca, 0xe1, 0xfe, 0xdb, 0x00, 0xb8, 0xf5, 0x80, 0x47, 0xf1, 0xc9, 0x22, 0x01, 0x22, 0x01, 0x03, 0x01, 0x0f, 0x01, 0x42, 0x80, 0x0a, 0x02, 0x26, 0x01, 0x26, 0x01};
const uint8_t DBL_DATA[] = {0x21, 0x13, 0xff, 0x11, 0x00, 0x2c, 0x0e, 0xeb, 0x7e, 0x12, 0x23, 0x13, 0x0d, 0xc2, 0x08, 0xff, 0xc3, 0x00, 0x2c, 0xf3, 0xaf, 0xd3, 0x22, 0x2f, 0xd3, 0x23, 0x3e, 0x2c, 0xd3, 0x22, 0x3e, 0x03, 0xd3, 0x10, 0xdb, 0xff, 0xe6, 0x10, 0x0f, 0x0f, 0xc6, 0x10, 0xd3, 0x10, 0x31, 0x79, 0x2d, 0xaf, 0xd3, 0x08, 0xdb, 0x08, 0xe6, 0x08, 0xc2, 0x1c, 0x2c, 0x3e, 0x04, 0xd3, 0x09, 0xc3, 0x38, 0x2c, 0xdb, 0x08, 0xe6, 0x02, 0xc2, 0x2d, 0x2c, 0x3e, 0x02, 0xd3, 0x09, 0xdb, 0x08, 0xe6, 0x40, 0xc2, 0x2d, 0x2c, 0x11, 0x00, 0x00, 0x06, 0x00, 0x3e, 0x10, 0xf5, 0xd5, 0xc5, 0xd5, 0x11, 0x86, 0x80, 0x21, 0xeb, 0x2c, 0xdb, 0x09, 0x1f, 0xda, 0x50, 0x2c, 0xe6, 0x1f, 0xb8, 0xc2, 0x50, 0x2c, 0xdb, 0x08, 0xb7, 0xfa, 0x5c, 0x2c, 0xdb, 0x0a, 0x77, 0x23, 0x1d, 0xca, 0x72, 0x2c, 0x1d, 0xdb, 0x0a, 0x77, 0x23, 0xc2, 0x5c, 0x2c, 0xe1, 0x11, 0xee, 0x2c, 0x01, 0x80, 0x00, 0x1a, 0x77, 0xbe, 0xc2, 0xcb, 0x2c, 0x80, 0x47, 0x13, 0x23, 0x0d, 0xc2, 0x79, 0x2c, 0x1a, 0xfe, 0xff, 0xc2, 0x90, 0x2c, 0x13, 0x1a, 0xb8, 0xc1, 0xeb, 0xc2, 0xc2, 0x2c, 0xf1, 0xf1, 0x2a, 0xec, 0x2c, 0xcd, 0xe5, 0x2c, 0xd2, 0xbb, 0x2c, 0x04, 0x04, 0x78, 0xfe, 0x20, 0xda, 0x44, 0x2c, 0x06, 0x01, 0xca, 0x44, 0x2c, 0xdb, 0x08, 0xe6, 0x02, 0xc2, 0xad, 0x2c, 0x3e, 0x01, 0xd3, 0x09, 0xc3, 0x42, 0x2c, 0x3e, 0x80, 0xd3, 0x08, 0xc3, 0x00, 0x00, 0xd1, 0xf1, 0x3d, 0xc2, 0x46, 0x2c, 0x3e, 0x43, 0x01, 0x3e, 0x4d, 0xfb, 0x32, 0x00, 0x00, 0x22, 0x01, 0x00, 0x47, 0x3e, 0x80, 0xd3, 0x08, 0x78, 0xd3, 0x01, 0xd3, 0x11, 0xd3, 0x05, 0xd3, 0x23, 0xc3, 0xda, 0x2c, 0x7a, 0xbc, 0xc0, 0x7b, 0xbd, 0xc9, 0x00, 0x00};

const rom_t TURMON =   {0xfd00, sizeof(TURMON_DATA), TURMON_DATA};
// H replaces DUMP functionality with Intel HEX load
const rom_t TURMONH =   {0xfd00, sizeof(TURMONH_DATA), TURMONH_DATA};
const rom_t MBL =      {0xfe00, sizeof(MBL_DATA), MBL_DATA};
const rom_t DBL =      {0xff00, sizeof(DBL_DATA), DBL_DATA};

void reset(bool add_default_ports, const rom_t *rom)
{
    i8080_reset(&cpu);
    irq_controller_out(0x00);
    
    if(rom)
        memory_load_rom(rom);
    
    if(add_default_ports)
    {
        port_io_t port;
        
        port.in = serial1_status;
        port.out = serial1_control;
        i8080_assign_port(&cpu, 0x0, &port);
        i8080_assign_port(&cpu, 0x10, &port);
        port.in = serial1_in;
        port.out = char_out;
        i8080_assign_port(&cpu, 0x1, &port);
        i8080_assign_port(&cpu, 0x11, &port);
        
        port.in = serial2_status;
        port.out = serial2_control;
        i8080_assign_port(&cpu, 0x12, &port);
        port.in = serial2_in;
        port.out = serial2_out;
        i8080_assign_port(&cpu, 0x13, &port);
        
        port.in = floppy_disk_status;
        port.out = floppy_disk_select;
        i8080_assign_port(&cpu, 0x8, &port);
        port.in = floppy_sector;
        port.out = floppy_disk_function;
        i8080_assign_port(&cpu, 0x9, &port);
        port.in = floppy_read;
        port.out = floppy_write;
        i8080_assign_port(&cpu, 0xA, &port);
        
        
        port.out = irq_controller_out;
        port.in = 0;
        i8080_assign_port(&cpu, 0xfe, &port);
        
        port.in = sense_sw;
        port.out = 0;
        i8080_assign_port(&cpu, 0xff, &port);
        
    }
}

bool wait_for_output(const char *inp, uint32_t timeout)
{
    START_TIMEOUT(timeout);
    while(!match_output(inp, NULL, NULL))
    {
        i8080_cycle(&cpu);
        if(TIMEOUT())
        {
            return false;
        }
    }
    return true;
}

bool wait_for_output_with_error(const char* inp, const char *error, uint32_t timeout)
{
    START_TIMEOUT(timeout);
    uint8_t error_out = 0;
    while (!match_output(inp, error, &error_out))
    {
        i8080_cycle(&cpu);
        if (error_out || TIMEOUT())
        {
            return false;
        }
    }
    return true;
}

void run_command(const char *inp)
{
    serial1_add_data((uint8_t*)inp, strlen(inp));
    serial1_add_data((uint8_t*)"\n", 1);
}

#define DEFAULT_TIMEOUT 2000
#define DEFAULT_BUILD_TIMEOUT 30000
#define INFINITE_TIMEOUT 0x10000000

#define RUN_TEST(name, input, output, error, timeout) do { if(input) { TEST(run_command(input)); } if(error) { TEST_CONDITION(wait_for_output_with_error(output, error, timeout), name); } else { TEST_CONDITION(wait_for_output(output, timeout), name); } } while(0)
#define WAIT_TEST(name, output, timeout) do { TEST_CONDITION(wait_for_output(output, timeout), name); } while(0)

bool cpm_tests(test_results_t *test_results, const char *boot_string)
{
    reset(true, &DBL);
    i8080_examine(&cpu, 0xff00);
    
    START_TEST("boot CP/M", TEST_LEVEL_ONE);
    WAIT_TEST("boot", boot_string, DEFAULT_TIMEOUT);
    WAIT_TEST("prompt", "A>", DEFAULT_TIMEOUT);
    END_TEST();
    
    START_TEST("ls.com", TEST_LEVEL_ONE);
    //run_command("ls");
    //while (1) { i8080_cycle(&cpu); }
    RUN_TEST("ls", "LS", "bytes remain on A:", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("prompt", "A>", DEFAULT_TIMEOUT);
    END_TEST();
    
    START_TEST("change disk", TEST_LEVEL_ONE);
    RUN_TEST("change drive", "C:", "C>", NULL, DEFAULT_TIMEOUT);
    RUN_TEST("ls", "ls", "bytes remain on C:", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("prompt", "C>", DEFAULT_TIMEOUT);
    END_TEST();
    
    START_TEST("c compiler", TEST_LEVEL_ONE);
    RUN_TEST("compile", "cc hello.c", " to spare", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("prompt", "C>", DEFAULT_TIMEOUT);
    RUN_TEST("link", "clink hello", "link space remaining", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("prompt", "C>", DEFAULT_TIMEOUT);
    RUN_TEST("run binary", "HELLO", "Altair 8800", NULL, DEFAULT_TIMEOUT);
    END_TEST();
    
    START_TEST("reboot", TEST_LEVEL_ONE);
    TEST(reset(true, &DBL));
    TEST(i8080_examine(&cpu, 0xff00));
    WAIT_TEST("boot", boot_string, DEFAULT_TIMEOUT);
    WAIT_TEST("prompt", "A>", DEFAULT_TIMEOUT);
    RUN_TEST("change drive", "C:", "C>", NULL, DEFAULT_TIMEOUT);
    RUN_TEST("ls", "ls", "bytes remain on C:", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("prompt", "C>", DEFAULT_TIMEOUT);
    RUN_TEST("run binary", "HELLO", "Altair 8800", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("prompt", "C>", DEFAULT_TIMEOUT);
    END_TEST();
    
    return true;
}


DECLARE_TEST(tape_basic)
{
    reset(true, &MBL);
    START_TEST("open tape file", TEST_LEVEL_ONE);
    TEST_CONDITION(file_open_temporary(&serial2_tape_file, "test/test_data/8K Basic Ver 4-0.tap", FILE_FLAG_READ | FILE_FLAG_OPEN), "tape opened");
    END_TEST();
    i8080_examine(&cpu, MBL.offset);
    sense_switches = 0b00011110;
    serial1_clear_buffer();
    
    START_TEST("boot basic from tape", TEST_LEVEL_ONE);
    WAIT_TEST("basic boot", "MEMORY SIZE?", DEFAULT_TIMEOUT);
    RUN_TEST("set memory", "\r\n", "TERMINAL WIDTH?", NULL, DEFAULT_TIMEOUT);
    RUN_TEST("set terminal", "80\r\n", "WANT SIN-COS-TAN-ATN?", NULL, DEFAULT_TIMEOUT);
    RUN_TEST("set geo func", "Y\r\n", "OK", NULL, DEFAULT_TIMEOUT);
    END_TEST();
    START_TEST("run basic program", TEST_LEVEL_ONE);
    RUN_TEST("simple program", "PRINT SIN(3.1415926)\r\n", "1.87254E-07", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("prompt", "OK", DEFAULT_TIMEOUT);
    END_TEST();
    
    return true;
}

DECLARE_TEST(cpm22)
{
    START_TEST("open disk file", TEST_LEVEL_ONE);
    TEST_CONDITION(file_open_temporary(&floppy_disk_drive.disks[0].fp, "test/test_data/cpm63k.dsk", FILE_FLAG_READ | FILE_FLAG_OPEN | FILE_FLAG_WRITE), "disk 0 opened");
    TEST_CONDITION(file_open_temporary(&floppy_disk_drive.disks[1].fp, "test/test_data/sysdev.dsk", FILE_FLAG_READ | FILE_FLAG_OPEN | FILE_FLAG_WRITE), "disk 1 opened");
    TEST_CONDITION(file_open_temporary(&floppy_disk_drive.disks[2].fp, "test/test_data/bdsc.dsk", FILE_FLAG_READ | FILE_FLAG_OPEN | FILE_FLAG_WRITE), "disk 2 opened");
    END_TEST();
    
    serial1_clear_buffer();
    
    cpm_tests(test_results, "Burcon Inc.");
    
    START_TEST("build cp/m2.2", TEST_LEVEL_TWO);
    RUN_TEST("change drive", "B:", "B>", NULL, DEFAULT_TIMEOUT);
    RUN_TEST("compile cp/m", "submit movcpm.sub", "RUNNING THE SYSGEN COMMAND", NULL, DEFAULT_BUILD_TIMEOUT);
    WAIT_TEST("prompt", "B>", DEFAULT_TIMEOUT);
    END_TEST();
    
    return true;
}

DECLARE_TEST(cpm30)
{
    START_TEST("open disk file", TEST_LEVEL_ONE);
    TEST_CONDITION(file_open_temporary(&floppy_disk_drive.disks[0].fp, "test/test_data/cpm3_v1.0_56K_disk1.dsk", FILE_FLAG_READ | FILE_FLAG_OPEN | FILE_FLAG_WRITE), "disk 0 opened");
    TEST_CONDITION(file_open_temporary(&floppy_disk_drive.disks[1].fp, "test/test_data/cpm3_v1.0_56k_build.dsk", FILE_FLAG_READ | FILE_FLAG_OPEN | FILE_FLAG_WRITE), "disk 1 opened");
    TEST_CONDITION(file_open_temporary(&floppy_disk_drive.disks[2].fp, "test/test_data/bdsc.dsk", FILE_FLAG_READ | FILE_FLAG_OPEN | FILE_FLAG_WRITE), "disk 2 opened");
    END_TEST();
    
    serial1_clear_buffer();
    
    cpm_tests(test_results, "Altair 8\" Floppy");
    
    START_TEST("build cp/m3", TEST_LEVEL_TWO);
    RUN_TEST("change drive", "B:", "B>", NULL, DEFAULT_TIMEOUT);
    RUN_TEST("compile cp/m", "submit makecpm.sub", "*** CP/M 3.0 SYSTEM GENERATION DONE ***", NULL, DEFAULT_BUILD_TIMEOUT);
    WAIT_TEST("almost done", "COPYSYS CPM3LDR.COM", DEFAULT_TIMEOUT);
    WAIT_TEST("prompt", "B>", DEFAULT_TIMEOUT);
    END_TEST();
    
    START_TEST("format disk", TEST_LEVEL_ONE);
    RUN_TEST("change drive", "A:", "A:", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("change drive complete", "A>", DEFAULT_TIMEOUT);
    RUN_TEST("start format", "AFORMAT B:", "(Y/N)?", NULL, DEFAULT_TIMEOUT);
    RUN_TEST("format running", "Y", "8\" Disk", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("format done", " another (Y/N)?", DEFAULT_TIMEOUT);
    RUN_TEST("exiting format", "N", "Ready (Y/N)?", NULL, DEFAULT_TIMEOUT);
    RUN_TEST("prompt", "Y", "A>", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("prompt 2", "\n", DEFAULT_TIMEOUT);
    WAIT_TEST("prompt 3", "A>", DEFAULT_TIMEOUT);
    RUN_TEST("change disk", "B:", "B>", NULL, DEFAULT_TIMEOUT);
    RUN_TEST("list dir", "dir", "No File", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("prompt", "B>", DEFAULT_TIMEOUT);
    END_TEST();
    return true;
}

DECLARE_TEST(rtc_interrupt)
{
    reset(true, &TURMONH);
    memory_load_rom(&MBL);
    START_TEST("open tape file", TEST_LEVEL_ONE);
    TEST_CONDITION(file_open_temporary(&serial2_tape_file, "test/test_data/8K Basic Ver 4-0.tap", FILE_FLAG_READ | FILE_FLAG_OPEN), "tape opened");
    END_TEST();
    i8080_examine(&cpu, TURMONH.offset);
    sense_switches = 0b00011110;
    serial1_clear_buffer();
    
    irq_rtc_set_line(7);
    irq_rtc_set_divider(1);
    
    START_TEST("load monitor", TEST_LEVEL_ONE);
    WAIT_TEST("monitor loaded", ".", DEFAULT_TIMEOUT);
    END_TEST();
    
    START_TEST("enter hex file", TEST_LEVEL_ONE);
    RUN_TEST("hex file loading", "H:101FBB00F5C5E53AF91FF53E0832F91FF6D8D3FE01\n" 
             ":101FCB00FB060321F41F7E34DE3BC2E61F7723059D\n"
             ":101FDB00C2D11F7E34DE17C2E61F77F3F132F91F31\n"
             ":091FEB00F6C0D3FEE1C1F1FBC90F\n"
             ":071FF900003EF0D3FEFBC91E\n"
             ":0000000000\n", "::", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("hex file loaded", ".", DEFAULT_TIMEOUT);
    END_TEST();
    
    i8080_examine(&cpu, MBL.offset);
    START_TEST("load basic from tape", TEST_LEVEL_ONE);
    WAIT_TEST("basic boot", "MEMORY SIZE?", DEFAULT_TIMEOUT);
    RUN_TEST("set memory", "8122\r\n", "TERMINAL WIDTH?", NULL, DEFAULT_TIMEOUT);
    RUN_TEST("set terminal", "80\r\n", "WANT SIN-COS-TAN-ATN?", NULL, DEFAULT_TIMEOUT);
    RUN_TEST("set geo func", "N\r\n", "OK", NULL, DEFAULT_TIMEOUT);
    END_TEST();
    
    START_TEST("setup rtc", TEST_LEVEL_ONE);
    RUN_TEST("set interrupt instr", "POKE 56,195\r\n", "POKE", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("ok", "OK", DEFAULT_TIMEOUT);
    RUN_TEST("set offset 1", "POKE 57,187\r\n", "POKE", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("ok", "OK", DEFAULT_TIMEOUT);
    RUN_TEST("set offset 2", "POKE 58,31\r\n", "POKE", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("ok", "OK", DEFAULT_TIMEOUT);
    RUN_TEST("set user func 1", "POKE 73,250\r\n", "POKE", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("ok", "OK", DEFAULT_TIMEOUT);
    RUN_TEST("set user func 2", "POKE 74,31\r\n", "POKE", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("ok", "OK", DEFAULT_TIMEOUT);
    RUN_TEST("set ms", "POKE 8180,00\r\n", "POKE", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("ok", "OK", DEFAULT_TIMEOUT);
    RUN_TEST("set s", "POKE 8181,00\r\n", "POKE", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("ok", "OK", DEFAULT_TIMEOUT);
    RUN_TEST("set m", "POKE 8182,00\r\n", "POKE", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("ok", "OK", DEFAULT_TIMEOUT);
    RUN_TEST("set h", "POKE 8183,00\r\n", "POKE", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("ok", "OK", DEFAULT_TIMEOUT);
    RUN_TEST("init interrupt", "A = USR(1)\r\n", "USR", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("ok", "OK", DEFAULT_TIMEOUT);
    END_TEST();
    
    START_TEST("test interrupt", TEST_LEVEL_ONE);
    TEST(irq_rtc_fire());
    TEST(i8080_cycle(&cpu));
    TEST(i8080_cycle(&cpu));
    TEST_CONDITION(cpu.address_bus == 070, "rtc interrupt running");
    TEST(i8080_cycle(&cpu));
    TEST_CONDITION(cpu.address_bus == 8123, "rtc service routine running");
    END_TEST();
    
    START_TEST("read rtc from basic", TEST_LEVEL_ONE);
    RUN_TEST("run program", "10 DIM Z(3)\r\n"
             "20 FOR X=1 TO 3\r\n"
             "30 Z(X)=PEEK(8180+X)\r\n"
             "40 NEXT X\r\n"
             "50 PRINTZ(3);\":\";Z(2);\":\";Z(1)\r\n"
             "RUN\r\n", "0 : 0 : 0", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("ok", "OK", DEFAULT_TIMEOUT);
    TEST(for(int i = 0; i < 60; i++)
         {
             irq_rtc_fire();
             for(int i = 0; i < 200; i++)
             {
                 i8080_cycle(&cpu);
             }
         });
    RUN_TEST("run command again", "RUN\r\n", "0 : 0 : 1", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("ok", "OK", DEFAULT_TIMEOUT);
    END_TEST();
    
    return true;
}

DECLARE_TEST(cpu_tests)
{
    START_TEST("open disk file", TEST_LEVEL_ONE);
    TEST_CONDITION(file_open_temporary(&floppy_disk_drive.disks[0].fp, "test/test_data/cpm63k.dsk", FILE_FLAG_READ | FILE_FLAG_OPEN | FILE_FLAG_WRITE), "disk 0 opened");
    TEST_CONDITION(file_open_temporary(&floppy_disk_drive.disks[1].fp, "test/test_data/cpu_tests.dsk", FILE_FLAG_READ | FILE_FLAG_OPEN | FILE_FLAG_WRITE), "disk 1 opened");
    END_TEST();
    
    reset(true, &DBL);
    sense_switches = 0xff;
    i8080_examine(&cpu, 0xff00);
    serial1_clear_buffer();
    
    //while (true) { i8080_cycle(&cpu); }
    
    START_TEST("boot CP/M", TEST_LEVEL_ONE);
    WAIT_TEST("boot", "Burcon", DEFAULT_TIMEOUT);
    WAIT_TEST("prompt", "A>", DEFAULT_TIMEOUT);
    RUN_TEST("change drive", "B:", "B>", NULL, DEFAULT_TIMEOUT);
    END_TEST();
    
    START_TEST("CPUTEST", TEST_LEVEL_ONE);
    RUN_TEST("pattern", "CPUTEST", "ABCDEFGHIJKLMNOPQRSTUVWXYZ", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("cpu validated", "CPU IS 8080/8085", DEFAULT_TIMEOUT);
    WAIT_TEST("timing test started", "BEGIN TIMING TEST", DEFAULT_TIMEOUT);
    WAIT_TEST("timing test ended", "END TIMING TEST", DEFAULT_TIMEOUT*4);
    WAIT_TEST("cpu passed", "CPU TESTS OK", DEFAULT_TIMEOUT*2);
    WAIT_TEST("prompt", "B>", DEFAULT_TIMEOUT);
    END_TEST();
    
    START_TEST("TST8080", TEST_LEVEL_ONE);
    RUN_TEST("cpu ok", "TST8080", "CPU IS OPERATIONAL", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("exit", "B>", DEFAULT_TIMEOUT);
    END_TEST();
    
    START_TEST("8080PRE", TEST_LEVEL_ONE);
    RUN_TEST("test ok", "8080PRE", "8080 Preliminary tests complete", NULL, DEFAULT_TIMEOUT);
    WAIT_TEST("exit", "B>", DEFAULT_TIMEOUT);
    END_TEST();
    
#define EXM_TEST(name) TEST(wait_for_output(name, INFINITE_TIMEOUT)); \
TEST_CONDITION(wait_for_output_with_error("PASS!", "ERROR", INFINITE_TIMEOUT), name);
    
    START_TEST("8080EXM", TEST_LEVEL_THREE);
    TEST(run_command("8080EXM"));
    EXM_TEST("dad <b,d,h,sp>");
    EXM_TEST("aluop nn");
    EXM_TEST("aluop <b,c,d,e,h,l,m,a>");
    EXM_TEST("<daa,cma,stc,cmc>");
    EXM_TEST("<inr,dcr> a");
    EXM_TEST("<inr,dcr> b");
    EXM_TEST("<inr,dcr> c");
    EXM_TEST("<inr,dcr> d");
    EXM_TEST("<inx,dcx> d");
    EXM_TEST("<inr,dcr> e");
    EXM_TEST("<inr,dcr> h");
    EXM_TEST("<inr,dcr> l");
    EXM_TEST("<inr,dcr> m");
    EXM_TEST("<inx,dcx> sp");
    EXM_TEST("lhld nnnn");
    EXM_TEST("shld nnnn");
    EXM_TEST("lxi <b,d,h,sp>,nnnn");
    EXM_TEST("ldax <b,d>");
    EXM_TEST("mvi <b,c,d,e,h,l,m,a>,nn");
    EXM_TEST("mov <bcdehla>,<bcdehla>");
    EXM_TEST("sta nnnn / lda nnnn");
    EXM_TEST("<rlc,rrc,ral,rar>");
    EXM_TEST("stax <b,d>");
    TEST_CONDITION(wait_for_output("Tests complete", DEFAULT_TIMEOUT), "tests done");
    TEST_CONDITION(wait_for_output("B>", DEFAULT_TIMEOUT), "exited");
    END_TEST();
    
    return true;
}
