#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include "main.h"
#include "ff.h"
#include "sys/usb.h"
#include "sys/log.h"

// Some basic syscall implementations for newlib. 
// stdout is redirected to the virtual com port and stderr
// is redirected to the SWO trace line.

typedef int (*puts_t)(char *, int);

int swo_puts(char *buf, int len)
{
    for (int i = 0; i < len; i++)
    {
        ITM_SendChar(*buf++);
    }
    return len;
}

int serial_puts(char *buf, int len)
{
    if(usb_transmit((uint8_t*)buf, len))
    {
        return len;
    }
    return -1;
}

static const puts_t _out_funs[] =
{
    [STDIN_FILENO] = NULL,
    [STDOUT_FILENO] = serial_puts,
    [STDERR_FILENO] = swo_puts
};

int _write(int file, char *ptr, int len)
{
    puts_t put_str = NULL;

    if(file >= COUNTOF(_out_funs) || file < 0)
    {
        errno = EBADF;
        return -1;
    }
    
    put_str = _out_funs[file];

    if(put_str)
    {
        return put_str(ptr, len);
    }

    errno = EINVAL;
    return -1;
}

int _open(const char *name, int flags, ...)
{
    return -1;
}

/*void _exit();
int close(int file);*/
//char **environ; /* pointer to array of char * strings that define the current environment variables */
/*int execve(char *name, char **argv, char **env);
int fork();
int fstat(int file, struct stat *st);
int getpid();
int isatty(int file);
int kill(int pid, int sig);
int link(char *old, char *new);
int lseek(int file, int ptr, int dir);

int read(int file, char *ptr, int len);
caddr_t sbrk(int incr);
int stat(const char *file, struct stat *st);
clock_t times(struct tms *buf);
int unlink(char *name);
int wait(int *status);
int write(int file, char *ptr, int len);
int gettimeofday(struct timeval *p, struct timezone *z);*/