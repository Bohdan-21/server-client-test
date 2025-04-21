#include "buffer.h"


buffer_t* create_buffer(size_t buffer_size)
{
    buffer_t* buffer = get_mem(sizeof(buffer_t));

    buffer->ptr = get_mem(buffer_size);
    buffer->size = buffer_size;
    buffer->length = 0;

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
    return buffer->ptr + buffer->length;
}

size_t get_buffer_free_space(buffer_t* buffer)
{
    return buffer->size - buffer->length;
}

size_t get_buffer_content_length(buffer_t* buffer)
{
    return buffer->length;
}



void clear_buffer(buffer_t* buffer)
{
    buffer->length = 0;
}

int is_buffer_empty(buffer_t* buffer)
{
    return buffer->length == 0;
}

int find(buffer_t* buffer, char separator)
{
    int position = -1;

    for (int i = 0; i < buffer->length; i++)
    {
        if (buffer->ptr[i] == separator)
        {
            position = i;
            break;
        }
    }

    return position;
}




void move_content_left(buffer_t* buffer, int offset)
{
    int n;

    if (offset <= 0)
        return;    

    n = buffer->length - offset;

    memmove(buffer->ptr, buffer->ptr + offset, n);

    buffer->length = n;
}



void push_string_on_buffer(buffer_t* buffer, char* str)
{
    size_t length;
    char* end_point;

    end_point = get_buffer_end_pointer(buffer);

    length = make_copy_string(end_point, str);

    buffer->length += length;
}
