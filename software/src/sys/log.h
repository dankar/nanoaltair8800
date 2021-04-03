#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>
#include <stdint.h>

typedef uint8_t uint8_t;
typedef uint16_t uint16_t;
typedef uint32_t uint32_t;
typedef uint64_t uint64_t;

typedef int8_t int8_t;
typedef int16_t int16_t;
typedef int32_t int32_t;
typedef int64_t int64_t;

#ifdef DEBUG_BUILD
#define DEBUG_OUTPUT 
#endif

#define OUTPUT_ON  1
#define OUTPUT_OFF 0

#define COUNTOF(a) (sizeof(a)/sizeof((a[0])))

#ifdef DEBUG_OUTPUT
#define LOG_UNIT(x) static const char *__log_unit __attribute__((__unused__)) = x
#define LOG_TYPE(x, on) static const uint8_t __log_ ## x __attribute__((__unused__)) = on
extern const uint8_t __log_ALWAYS;
#define LOG(type, ...) do { if(__log_ ## type) {  fprintf(stderr, "(%s) ", __log_unit); fprintf(stderr, __VA_ARGS__); } } while(0)
#define ASSERT(cond, message) do { if(!(cond)) { LOG(ALWAYS, "Assert: " message ", \"" #cond "\" in " __FILE__ ":%d", __LINE__); __asm__ __volatile__("bkpt"); } } while(0)
#else
#define LOG_UNIT(x)
#define LOG_TYPE(x, on)
#define LOG(type, ...)
#define ASSERT(cond, msg)
#endif
#endif