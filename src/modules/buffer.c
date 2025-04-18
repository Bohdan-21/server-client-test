#include "buffer.h"


buffer_t* create_buffer()
{
    buffer_t* buffer = get_mem(sizeof(buffer_t));

    buffer->ptr = get_mem(BASE_SIZE);
    buffer->size = 0;

    return buffer;
}

void free_buffer(void* data)
{
    buffer_t* buffer = (buffer_t*)data;

    free(buffer->ptr);
    free(buffer);
}



char* get_buffer_start_pointer(buffer_t* buffer)
{
    return buffer->ptr;
}

char* get_buffer_end_pointer(buffer_t* buffer)
{
    return buffer->ptr + buffer->size;
}

size_t get_buffer_free_space(buffer_t* buffer)
{
    return BASE_SIZE - buffer->size;
}

size_t get_buffer_content_length(buffer_t* buffer)
{
    return buffer->size;
}



void clear_buffer(buffer_t* buffer)
{
    buffer->size = 0;
}

int is_buffer_empty(buffer_t* buffer)
{
    return buffer->size == 0;
}

int find(buffer_t* buffer, char separator)
{
    int position = -1;

    for (int i = 0; i < buffer->size; i++)
    {
        if (buffer->ptr[i] == separator)
        {
            position = i;
            break;
        }
    }

    return position;
}


/*need uptdate*/
/*memmove(buffer->ptr, buffer->ptr + offser, n)*/
/*where n = buffer->size - offset;*/
/*this function can work without allocated new mew*/
void move_content_left(buffer_t* buffer, int offset)
{
    int n;
    char* dest;

    if (offset <= 0)
        return;    

    n = buffer->size - offset;
    dest = get_mem(BASE_SIZE);

    memmove(dest, buffer->ptr + offset, n);

    free(buffer->ptr);

    buffer->ptr = dest;
    buffer->size = n;

    /*int i = 0;
    int j = offset;

    for (;i < buffer->size; i++, j++)
    {
        if (j >= buffer->size)
            break;
        else
            buffer->ptr[i] = buffer->ptr[j];
    }

    buffer->size -= offset;*/
}