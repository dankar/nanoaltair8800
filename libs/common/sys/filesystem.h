#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

#include <stdint.h>
#include <stdbool.h>
#ifdef TEST_BUILD
#include <stdio.h>
#else
#include "fatfs.h"
#endif

#define MAX_FAT_PATH 256

#define FILE_FLAG_READ    (0x1)
#define FILE_FLAG_WRITE   (0x2)
#define FILE_FLAG_OPEN    (0x4)
#define FILE_FLAG_CREATE  (0x8)

typedef struct file_t
{
#ifdef TEST_BUILD
    FILE *fp;
#else
    FIL fp;
#endif
} file_t;

bool file_open(file_t *fp, const char *filename, uint8_t flags);
bool file_read(file_t *fp, uint8_t *b);
bool file_write(file_t *fp, uint8_t b);
size_t file_size(file_t *fp);
bool file_read_buf(file_t *fp, uint8_t *b, size_t len);
bool file_write_buf(file_t *fp, uint8_t *b, size_t len);
bool file_seek(file_t *fp, uint32_t offset);
bool file_is_valid(file_t *fp);
bool file_eof(file_t *fp);
bool file_close(file_t *fp);

#endif