#ifndef _88DCDD_H_
#define _88DCDD_H_

#include "sys/filesystem.h"
#include "intel8080.h"

#define STATUS_ENWD            1
#define STATUS_MOVE_HEAD       2
#define STATUS_HEAD            4
#define STATUS_IE              32
#define STATUS_TRACK_0         64
#define STATUS_NRDA            128

#define CONTROL_STEP_IN        1
#define CONTROL_STEP_OUT       2
#define CONTROL_HEAD_LOAD      4
#define CONTROL_HEAD_UNLOAD    8
#define CONTROL_IE             16
#define CONTROL_ID             32
#define CONTROL_HCS            64
#define CONTROL_WE             128

#define SECTOR                 137UL
#define TRACK                  (32UL*SECTOR)

#define MAX_FLOPPY_DISKS       4

typedef struct floppy_disk_t
{
	file_t fp;
	uint8_t track;
	uint8_t sector;
	uint8_t status;
	uint8_t write_status;
} floppy_disk_t;

typedef struct floppy_disks
{
	floppy_disk_t disks[MAX_FLOPPY_DISKS];
	floppy_disk_t nodisk;
	floppy_disk_t *current;
	bool head_loaded;
} floppy_disks;

typedef struct disk_controller_t
{
	port_io_t status_select;
	port_io_t function_sector;
	port_io_t read_write;
} disk_controller_t;

extern floppy_disks floppy_disk_drive;

void floppy_disk_select(uint8_t b);
bool floppy_disk_status(uint8_t *b);
void floppy_disk_function(uint8_t b);
bool floppy_sector(uint8_t *b);
void floppy_write(uint8_t b);
bool floppy_read(uint8_t *b);

disk_controller_t floppy_get_controller();


#endif
