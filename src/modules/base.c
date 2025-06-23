#include "base.h"
/*
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
*/


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