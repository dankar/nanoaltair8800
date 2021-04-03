#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "config.h"
#include "altair8800/88dcdd.h"
#include "altair8800/memory.h"
#include "altair8800/serial.h"
#include "altair8800/irq_controller.h"
#include "altair8800/serial.h"
#include "sys/switches.h"
#include "sys/filesystem.h"
#include "sys/log.h"

LOG_UNIT("config");
LOG_TYPE(CONFIG_DEBUG, OUTPUT_ON);

// Try to mount the SD card. First check if the card detect pin is low,
// then make two attempts to mount the file system. Try to reinitialize
// the FATFS system in between attempts.
//
// NOTE: There is still a bug that causes sporadic HardFault when
// reinserting the SD card during operation.
bool mount_sd()
{
    WCHAR sd_path[4];
    for(int i = 0; i < 4; i++)
    {
        sd_path[i] = SDPath[i];
    }
    if(HAL_GPIO_ReadPin(SD_CD_GPIO_Port, SD_CD_Pin) == GPIO_PIN_RESET)
    {
        if(f_mount(&SDFatFS, sd_path, 1) != FR_OK)
        {
            MX_FATFS_DeInit();
            MX_FATFS_Init();
            if(f_mount(&SDFatFS, sd_path, 1) != FR_OK)
            {
                return false;
            }
        }
        
        return true;
    }
    
    return false;
}

bool config_get_char(file_t *fp, char *c)
{
    return file_read(fp, (uint8_t*)c);
}

// Read a string from the config, ending with either newline
// or a delimiter. Return the delimiter or newline in next_char
bool config_read_str(file_t *fp, char *str, size_t max_len, char *next_char)
{
    bool started = false;
    size_t len = 0;
    char c;
    while(true)
    {
        if(!config_get_char(fp, &c))
        {
            return false;
        }
        // Skip starting whitespace
        if(!started && isspace(c))
        {
            continue;
        }
        if(c == '#')
        {
            while(true)
            {
                if(!config_get_char(fp, &c))
                {
                    return false;
                }
                if(c == '\n')
                {
                    break;
                }
            }
            continue;
        }
        
        if(c == '=' || c == '\n')
        {
            *next_char = c;
            str[len] = '\0';
            len--;
            while(isspace((int)str[len]))
            {
                str[len--] = '\0';
            }
            
            break;
        }
        
        started = true;
        str[len++] = c;
        if(len > max_len)
        {
            return false;
        }
    }
    
    return true;
}

// Get a config line separated into setting and a value
bool config_read_line(file_t *fp, char *setting, char *value, size_t max_len)
{
    
    char c;
    if(!config_read_str(fp, setting, max_len, &c))
    {
        return false;
    }
    
    if(c != '=')
    {
        printf("ERROR: Expected '=' after setting name\r\n");
    }
    
    if(!config_read_str(fp, value, max_len, &c))
    {
        return false;
    }
    
    if(c != '\n')
    {
        printf("ERROR: Expected newline after setting value\r\n");
        return false;
    }
    return true;
}

void config_close_if_open(file_t *fp)
{
    if(file_is_valid(fp))
    {
        file_close(fp);
    }
}

bool config_open_file(file_t *fp, const char *default_path, const char *path, bool read_only)
{
    char buffer[515];
    if(strlen(default_path))
    {
        sprintf(buffer, "%s%s/%s", SDPath, default_path, path);
    }
    else
    {
        sprintf(buffer, "%s/%s", SDPath, path);
    }
    
    config_close_if_open(fp);
    
    if(!file_open(fp, buffer, read_only ? FILE_FLAG_READ | FILE_FLAG_OPEN : FILE_FLAG_READ | FILE_FLAG_WRITE | FILE_FLAG_OPEN))
    {
        printf("ERROR: Could not open \"%s\"\r\n", buffer);
        return false;
    }
    return true;
}

// Translate a port function name into a port struct
bool config_port_from_string(const char *str, port_io_t *port)
{
    if(strcmp(str, "serial1_control") == 0)
    {
        port->in = serial1_status;
        port->out = serial1_control;
        return true;
    }
    if(strcmp(str, "serial1_data") == 0)
    {
        port->in = serial1_in;
        port->out = serial1_out;
        return true;
    }
    if(strcmp(str, "serial2_control") == 0)
    {
        port->in = serial2_status;
        port->out = serial2_control;
        return true;
    }
    if(strcmp(str, "serial2_data") == 0)
    {
        port->in = serial2_in;
        port->out = serial2_out;
        return true;
    }
    
    if(strcmp(str, "sense_sw") == 0)
    {
        port->in = sense;
        port->out = NULL;
        return true;
    }
    if(strcmp(str, "floppy_select_status") == 0)
    {
        port->in = floppy_disk_status;
        port->out = floppy_disk_select;
        return true;
    }
    if(strcmp(str, "floppy_function_sector") == 0)
    {
        port->in = floppy_sector;
        port->out = floppy_disk_function;
        return true;
    }
    if(strcmp(str, "floppy_data") == 0)
    {
        port->in = floppy_read;
        port->out = floppy_write;
        return true;
    }
    if(strcmp(str, "vic") == 0)
    {
        LOG(CONFIG_DEBUG, "Setting vic\n");
        port->in = NULL;
        port->out = irq_controller_out;
        return true;
    }
    return false;
}

// Read the config file and parse it
bool config_read(const char *filename, intel8080_t *cpu)
{
    char setting_buffer[256];
    char value_buffer[256];
    char default_path[256] = "";
    file_t scratch_file = {0};
    file_t cfg_file = {0};
    char scratch_buffer[4096];
    size_t len = 256;
    
    memset(memory, 0, MEMORY_SIZE);
    memory_disable_rom();
    floppy_disk_drive.nodisk.status = 0xff;
    irq_controller_out(0x00);
    i8080_reset(cpu);
    
    
    if(!mount_sd())
    {
        printf("ERROR: Failed to mount SD card.\r\n");
        return false;
    }
    
    if(!config_open_file(&cfg_file, "", filename, true))
    {
        printf("ERROR: Could not open file '%s'\r\n", filename);
        return false;
    }
    
    while(config_read_line(&cfg_file, setting_buffer, value_buffer, len))
    {
        LOG(CONFIG_DEBUG, "\"%s\" = \"%s\"\n", setting_buffer, value_buffer);
        if(strcmp(setting_buffer, "default_path") == 0)
        {
            if(strcmp(value_buffer, ".") == 0 || strcmp(value_buffer, "/") == 0)
            {
                default_path[0] = '\0';
            }
            else
            {
                strcpy(default_path, value_buffer);
            }
            continue;
        }
        if(memcmp(setting_buffer, "disk[", 5) == 0)
        {
            int number = atoi(&setting_buffer[5]);
            if(number > MAX_FLOPPY_DISKS - 1)
            {
                printf("ERROR: Can not mount disk %d, max is %d\r\n", number, MAX_FLOPPY_DISKS - 1);
                continue;
            }
            
            config_open_file(&floppy_disk_drive.disks[number].fp, default_path, value_buffer, false);
            floppy_disk_drive.disks[number].sector = 0;
            floppy_disk_drive.disks[number].status = 0;
            floppy_disk_drive.disks[number].write_status = 0;
            floppy_disk_drive.disks[number].track = 0;
            continue;
        }
        if(strcmp(setting_buffer, "tape") == 0)
        {
            config_open_file(&serial2_tape_file, default_path, value_buffer, true);
            continue;
        }
        if(memcmp(setting_buffer, "port[", 5) == 0)
        {
            int port_number = strtol(&setting_buffer[5], NULL, 16);
            port_io_t port;
            if(!config_port_from_string(value_buffer, &port))
            {
                printf("ERROR: Unknown port device \"%s\"\r\n", value_buffer);
            }
            i8080_assign_port(cpu, port_number, &port);
            continue;
        }
        if(strcmp(setting_buffer, "run_state") == 0)
        {
            cpu->run_state = atoi(value_buffer);
            continue;
        }
        if(strcmp(setting_buffer, "start_address") == 0)
        {
            i8080_examine(cpu, strtol(value_buffer, 0, 16));
            continue;
        }
        if(memcmp(setting_buffer, "rom[", 4) == 0)
        {
            long offset = strtol(&setting_buffer[4], NULL, 16);
            
            if(config_open_file(&scratch_file, default_path, value_buffer, true))
            {
                uint32_t size = file_size(&scratch_file);
                
                if(size > 4096)
                {
                    printf("ERROR: Can not load ROMs larger than 4096 bytes (\"%s\" is %ld bytes)\r\n", value_buffer, size);
                    continue;
                }
                if(!file_read_buf(&scratch_file, (uint8_t*)scratch_buffer, size))
                {
                    printf("ERROR: Could not read \"%s\"\r\n", value_buffer);
                    file_close(&scratch_file);
                    continue;
                }
                file_close(&scratch_file);
                
                rom_t rom = {offset, size, (const uint8_t*)scratch_buffer};
                memory_load_rom(&rom);
            }
            continue;
        }
        if(strcmp(setting_buffer, "rtc_divider") == 0)
        {
            irq_rtc_set_divider(atoi(value_buffer));
            continue;
        }
        if(strcmp(setting_buffer, "rtc_interrupt_line") == 0)
        {
            irq_rtc_set_line(atoi(value_buffer));
            continue;
        }
        if(strcmp(setting_buffer, "serial1_interrupt_line") == 0)
        {
            serial1_set_interrupt_line(atoi(value_buffer));
            continue;
        }
        
        printf("ERROR: Unknown setting: \"%s\"\r\n", setting_buffer);
    }
    
    file_close(&cfg_file);
    
    return true;
}