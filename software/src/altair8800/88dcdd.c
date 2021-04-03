#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "88dcdd.h"
#include "sys/log.h"

LOG_UNIT("88cdcdd");
LOG_TYPE(DISK_DEBUG, OUTPUT_OFF);

floppy_disks floppy_disk_drive = {0};

bool floppy_valid()
{
	return file_is_valid(&floppy_disk_drive.current->fp);
}

void floppy_set_status(uint8_t bit)
{
	floppy_disk_drive.current->status &= ~bit;
}

void floppy_clear_status(uint8_t bit)
{
	floppy_disk_drive.current->status |= bit;
}

void floppy_disk_select(uint8_t b)
{
	uint8_t select = b & 0xf;
	LOG(DISK_DEBUG, "Active disk: %d\n", b);
	if(select < MAX_FLOPPY_DISKS && file_is_valid(&floppy_disk_drive.disks[select].fp))
	{
		floppy_disk_drive.current = &floppy_disk_drive.disks[select];
	}
	else
	{
		memset(&floppy_disk_drive.nodisk, 0, sizeof(floppy_disk_t));
		floppy_disk_drive.current = &floppy_disk_drive.nodisk;
		floppy_disk_drive.head_loaded = false;
	}
}

bool floppy_disk_status(uint8_t *b)
{
	LOG(DISK_DEBUG, "Returning status %d for disk\n",floppy_disk_drive.current->status);
	if(floppy_valid())
	{
		if(floppy_disk_drive.head_loaded)
		{
			floppy_set_status(STATUS_HEAD);
		}
		else
		{
			floppy_clear_status(STATUS_HEAD);
		}

		if (floppy_disk_drive.current->track == 0)
		{
			floppy_set_status(STATUS_TRACK_0);
		}
		else
		{
			floppy_clear_status(STATUS_TRACK_0);
		}
		
		*b = floppy_disk_drive.current->status;
	}
	else
	{
		*b = 0xff;
	}
	return true;
}

void floppy_disk_function(uint8_t b)
{
	LOG(DISK_DEBUG, "Disk function %d\n", b);
	if(!floppy_valid())
		return;

	if(b & CONTROL_STEP_IN)
	{
		floppy_disk_drive.current->track++;
		if(floppy_disk_drive.current->track != 0)
			floppy_clear_status(STATUS_TRACK_0);

		file_seek(&floppy_disk_drive.current->fp, TRACK * floppy_disk_drive.current->track);
		LOG(DISK_DEBUG, "Track seek to : %ld\n", TRACK *floppy_disk_drive.current->track);
	}
	if(b & CONTROL_STEP_OUT)
	{
		if(floppy_disk_drive.current->track > 0)
			floppy_disk_drive.current->track--;
		if(floppy_disk_drive.current->track == 0)
			floppy_set_status(STATUS_TRACK_0);
		file_seek(&floppy_disk_drive.current->fp, TRACK * floppy_disk_drive.current->track);
	}
	if(b & CONTROL_HEAD_LOAD)
	{
		LOG(DISK_DEBUG, "Head load\n");
		floppy_set_status(STATUS_HEAD);
		floppy_set_status(STATUS_NRDA);
		floppy_disk_drive.head_loaded = true;
	}
	if(b & CONTROL_HEAD_UNLOAD)
	{
		LOG(DISK_DEBUG, "Head unload\n");
		floppy_clear_status(STATUS_HEAD);
		floppy_disk_drive.head_loaded = false;
	}
	if(b & CONTROL_IE)
	{

	}
	if(b & CONTROL_ID)
	{
	}
	if(b & CONTROL_HCS)
	{
	}
	if(b & CONTROL_WE)
	{
		LOG(DISK_DEBUG, "Write enable\n");
		floppy_set_status(STATUS_ENWD);
		floppy_disk_drive.current->write_status = 0;
	}
}

bool floppy_sector(uint8_t *b)
{
	uint32_t seek;
	static bool flip_flop = false;

	if(!floppy_disk_drive.head_loaded)
	{
		return false;
	}

	if(flip_flop)
	{
		flip_flop = false;
		*b = (floppy_disk_drive.current->sector - 1) << 1;
		LOG(DISK_DEBUG, "Current sector: %d (%X) (bytes per track: %ld)\n",floppy_disk_drive.current->sector, *b, TRACK);
		return true;
	}

	flip_flop = true;
	if(!floppy_valid())
	{
		LOG(DISK_DEBUG, "Disk not valid\n");
		return false;
	}
		

	if(floppy_disk_drive.current->sector == 32)
	{
		floppy_disk_drive.current->sector = 0;
	}
	//current_sector = current_sector % 32;
	seek = floppy_disk_drive.current->track * TRACK + floppy_disk_drive.current->sector * (SECTOR);
	file_seek(&floppy_disk_drive.current->fp, seek);

	*b = floppy_disk_drive.current->sector << 1 | 1;
	LOG(DISK_DEBUG, "Current sector: %d (%X) (bytes per track: %ld)\n",floppy_disk_drive.current->sector, *b, TRACK);
	
	floppy_disk_drive.current->sector++;
	return true;
}

void floppy_write(uint8_t b)
{
	static uint32_t num = 0;
	LOG(DISK_DEBUG, "Write %d (byte in sector: %d)\n", b, floppy_disk_drive.current->write_status);

	if(!floppy_valid())
		return;

	if(floppy_disk_drive.current->write_status == 137)
	{
		floppy_disk_drive.current->write_status = 0;
		floppy_clear_status(STATUS_ENWD);
		LOG(DISK_DEBUG, "Disabling clear\n");
		return;
	}

	if(!file_write(&floppy_disk_drive.current->fp, b))
	{
		LOG(ALWAYS, "Error: write failed\n");
	}
	else
	{
		num++;
	}
	floppy_disk_drive.current->write_status++;
}

bool floppy_read(uint8_t *b)
{
	static uint32_t bytes = 0;
	bool res = true;

	if(!floppy_valid())
		return false;

	if(!file_read(&floppy_disk_drive.current->fp, b))
	{
		LOG(DISK_DEBUG, "Failed to read byte...\n");
		res = false;
	}
	bytes++;
	LOG(DISK_DEBUG, "Reading byte %lu (%x)\n", bytes, *b);
	return res;
}

disk_controller_t floppy_get_controller()
{
	disk_controller_t disk = {
		.status_select.out = floppy_disk_select,
		.status_select.in = floppy_disk_status,
		
		.function_sector.out = floppy_disk_function,
		.function_sector.in = floppy_sector,

		.read_write.out = floppy_write,
		.read_write.in = floppy_read
	};

	return disk;
}
