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



char* make_new_copy_string(const char* str, int length)
{
    char* result = get_mem(length);

    memmove(result, str, length);

    return result;
}
/*work only with c string, return length src, including '\0'*/
size_t make_copy_string(char* dest, const char* src)
{
    size_t length = strlen(src) + 1;/*including '\0'*/

    memmove(dest, src, length);

    return length;
}

void replace_symbol(char* str, int length, char pattern, char replace)
{
    for (int i = 0;i < length;i++)
    {
        if (str[i] == pattern)
            str[i] = replace;
    }
}




/*

static int get_separator_position(const char* str);


static int is_have_separator_old(const char* str, int length)
{
    int i = 0;
    int separator_position = -1;

    for (; i < length; i++)
    {
        if (str[i] == DIRTY_STRING_SEPARATOR)
        {
            separator_position = i;
            break;
        }
    }
    return separator_position;
}

static char* make_copy_string_old(const char* str)
{
    size_t size;
    char *new_str;

    size = get_separator_position(str) + 1;*//*include separator*/
    /*
    new_str = get_mem(size);

    memmove(new_str, str, size);

    return new_str;
}

static int make_copy_string_to_string_old(char* dest, const char* src)
{
    int length = get_separator_position(src) + 1;

    memmove(dest, src, length);

    return length;
}


static char* make_c_string_old(char* str)
{
    int i;

    i = get_separator_position(str);
    
    str[i] = 0;

    return str;
}


static int get_separator_position(const char* str)
{
    int i = 0;

    while(str[i] != DIRTY_STRING_SEPARATOR)
        i++;
    
    return i;
}
*/