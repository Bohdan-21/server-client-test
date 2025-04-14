#include "custom_io.h"


int read_from_fd(buffer_t* destination, int source)
{
    int result_read;
    int free_space;
    char* pointer;

    free_space = get_buffer_free_space(destination);
    pointer = get_buffer_end_pointer(destination);

    if (!free_space)
    {
        perror("read_from_fd free_space");
        /*exit(EXIT_FAILURE);*/
        return -1;
    }

    result_read = read(source, pointer, free_space);

    if (result_read == -1)
    {
        perror("read_from_fd result_read");
        /*exit(EXIT_FAILURE);*/
        return result_read;
    }
    /*update_buffer_content_length(destination, result_read);*/destination->size+=result_read;

    return result_read;
}

int write_to_fd(int destination, buffer_t* source)
{
    int result_write;
    int content_length;
    char* pointer;

    content_length = get_buffer_content_length(source);
    pointer = get_buffer_start_pointer(source);

    result_write = write(destination, pointer, content_length);

    if (result_write == -1)
    {
        perror("write_to_fd result_write");
        /*exit(EXIT_FAILURE);*/
        return result_write;
    }

    /*removal_content_left_n_buffer(source, result_write);*/move_content_left(source, result_write);

    return result_write;
}

