#include "sys/filesystem.h"
#include <string.h>
#include <errno.h>
#ifdef _WIN32
#include <windows.h>
#endif

bool file_open(file_t *fp, const char *filename, uint8_t flags)
{
    const char *mode;
    
    if(flags == (FILE_FLAG_READ | FILE_FLAG_WRITE | FILE_FLAG_OPEN))
    {
        mode = "r+b";
    }
    else if(flags == (FILE_FLAG_READ | FILE_FLAG_OPEN))
    {
        mode = "rb";
    }
    else if(flags == (FILE_FLAG_READ | FILE_FLAG_WRITE | FILE_FLAG_CREATE))
    {
        mode = "wb";
    }
    else
    {
        mode = "rb";
    }
    
    fp->fp = fopen(filename, mode);
    
    return fp->fp != NULL;
}
bool file_read(file_t *fp, uint8_t *b)
{
    return fread(b, 1, 1, fp->fp) == 1;
}
bool file_read_buf(file_t *fp, uint8_t *b, size_t len)
{
    return fread(b, 1, len, fp->fp) == len;
}
bool file_write_buf(file_t *fp, uint8_t *b, size_t len)
{
    return fwrite(b, 1, len, fp->fp) == len;
}
bool file_write(file_t *fp, uint8_t b)
{
    return fwrite(&b, 1, 1, fp->fp) == 1;
}
size_t file_size(file_t *fp)
{
    size_t current_pos = ftell(fp->fp);
    
    fseek(fp->fp, 0, SEEK_END);
    
    size_t size = ftell(fp->fp);
    
    fseek(fp->fp, current_pos, SEEK_SET);
    
    return size;
}
bool file_seek(file_t *fp, uint32_t offset)
{
    fseek(fp->fp, offset, SEEK_SET);
    return true;
}

bool file_is_valid(file_t *fp)
{
    return fp->fp != NULL;
}

bool file_eof(file_t *fp)
{
    return feof(fp->fp);
}
bool file_close(file_t *fp)
{
    fclose(fp->fp);
    memset(fp, 0, sizeof(file_t));
    return true;
}