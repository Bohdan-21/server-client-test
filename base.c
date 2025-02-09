#include "base.h"

void* get_mem(size_t size)
{
    void* new_allocated_mem;

    new_allocated_mem = malloc(size);

    if (!new_allocated_mem)
    {
        perror("get_mem");
        exit(EXIT_FAILURE);
    }

    return new_allocated_mem;
}