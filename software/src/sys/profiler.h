#ifndef _PROFILER_H_
#define _PROFILER_H_

#include <stdint.h>
#include <stdbool.h>

#define NO_PROFILE __attribute__((no_instrument_function))

void NO_PROFILE profiler_print();
void NO_PROFILE profiler_reset();

#endif