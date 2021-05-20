#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdint.h>
#include <stdbool.h>

#define BUFFER_SIZE 256

typedef struct buffer_t
{
    uint16_t head;
    uint16_t tail;
    uint8_t data[BUFFER_SIZE];
} buffer_t;

bool buffer_is_empty(volatile buffer_t *buffer);
bool buffer_is_full(volatile buffer_t *buffer);
bool buffer_pop(volatile buffer_t *buffer, uint8_t *c);
bool buffer_push(volatile buffer_t *buffer, uint8_t c);

#endif