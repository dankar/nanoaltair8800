#include "buffer.h"

bool buffer_is_empty(volatile buffer_t *buffer)
{
    return buffer->head == buffer->tail;
}

bool buffer_is_full(volatile buffer_t *buffer)
{
    uint16_t new_head = (buffer->head + 1) % BUFFER_SIZE;
    if(new_head == buffer->tail)
    {
        return true;
    }

    return false;
}

bool buffer_pop(volatile buffer_t *buffer, uint8_t *c)
{
    if(buffer_is_empty(buffer))
    {
        return false;
    }
    
    *c = buffer->data[buffer->tail++];

    buffer->tail = buffer->tail % BUFFER_SIZE;

    return true;
}

bool buffer_push(volatile buffer_t *buffer, uint8_t c)
{
    if(buffer_is_full(buffer))
    {
        return false;
    }

    buffer->data[buffer->head++] = c;
    buffer->head = buffer->head % BUFFER_SIZE;

    return true;
}
