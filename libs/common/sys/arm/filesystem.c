#include "sys/filesystem.h"
#include <string.h>

bool file_open(file_t *fp, const char *filename, uint8_t flags)
{
    WCHAR wide_filename[MAX_FAT_PATH];
    BYTE mode = 0;
    if(flags & FILE_FLAG_READ)
        mode |= FA_READ;
    if(flags & FILE_FLAG_WRITE)
        mode |= FA_WRITE;
    if(flags & FILE_FLAG_OPEN)
        mode |= FA_OPEN_EXISTING;
    if(flags & FILE_FLAG_CREATE)
        mode |= FA_CREATE_NEW;
    
    if(strlen(filename) >= MAX_FAT_PATH)
    {
        return false;
    }
    
    for(int i = 0; i < strlen(filename); i++)
    {
        char b = filename[i];
        wide_filename[i] = b;
    }
    
    wide_filename[strlen(filename)] = '\0';
    
    if(f_open(&fp->fp, wide_filename, mode) != FR_OK)
    {
        memset(fp, 0, sizeof(file_t));
        return false;
    }
    
    return true;
}
bool file_read(file_t *fp, uint8_t *b)
{
    UINT r;
    FRESULT res = f_read(&fp->fp, b, 1, &r);
    
    return res == FR_OK && r == 1;
}

bool file_read_buf(file_t *fp, uint8_t *b, size_t len)
{
    UINT r;
    FRESULT res = f_read(&fp->fp, b, len, &r);
    return res == FR_OK && r == len;
}

bool file_write_buf(file_t *fp, uint8_t *b, size_t len)
{
    UINT r;
    FRESULT res = f_write(&fp->fp, b, len, &r);
    return res == FR_OK && r == len;
}


size_t file_size(file_t *fp)
{
    return fp->fp.obj.objsize;
}
bool file_write(file_t *fp, uint8_t b)
{
    UINT w;
    FRESULT res = f_write(&fp->fp, &b, 1, &w);
    
    return res == FR_OK && w == 1;
}

bool file_seek(file_t *fp, uint32_t offset)
{
    FRESULT res = f_lseek(&fp->fp, offset);
    return res == FR_OK;
}

bool file_is_valid(file_t *fp)
{
    return fp->fp.obj.fs != 0;
}

bool file_eof(file_t *fp)
{
    return f_eof(&fp->fp);
}

bool file_close(file_t *fp)
{
    f_close(&fp->fp);
    memset(fp, 0, sizeof(file_t));
    return true;
}