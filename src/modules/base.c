#include "base.h"

static int get_separator_position(const char* str);

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

int is_have_separator(const char* str, int length)
{
    int i = 0;
    int separator_position = -1;

    for (; i < length; i++)
    {
        if (str[i] == STRING_SEPARATOR)
        {
            separator_position = i;
            break;
        }
    }
    return separator_position;
}

char* make_copy_string(const char* str)
{
    size_t size;
    char *new_str;

    size = get_separator_position(str) + 1;/*include separator*/
    
    new_str = get_mem(size);

    memmove(new_str, str, size);

    return new_str;
}

char* make_c_string(char* str)
{
    int i;

    i = get_separator_position(str);
    
    str[i] = 0;

    return str;
}


static int get_separator_position(const char* str)
{
    int i = 0;

    while(str[i] != STRING_SEPARATOR)
        i++;
    
    return i;
}
