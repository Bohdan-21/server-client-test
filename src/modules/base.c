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

char* make_copy_string(const char* str)
{
    size_t size;
    char *new_str;

    size = strlen(str) + 1;/*+ 1 for include "\0" */
    new_str = get_mem(size);

    strcpy(new_str, str);

    return new_str;
}



