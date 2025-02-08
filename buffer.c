#include "buffer.h"

static void move_left_n(buffer_t* buffer, int n);
static char* get_mem();

static int get_length_free_space_in_buffer(buffer_t*);
static char* get_last_position_in_buffer(buffer_t*);
static void update_buffer_length(buffer_t*, int);
static int find_string(buffer_t*, char);

void initialize_buffer(buffer_t* buffer)
{
    buffer->buffer = get_mem(BUFFER_SIZE);

    buffer->length = 0;
}

void free_buffer(buffer_t* buffer)
{
    free(buffer->buffer);
}



void write_to_fd_from_buffer(int destination, buffer_t* source)
{
    int result_write;

    result_write = write(destination, source->buffer, source->length);

    if (result_write == -1)
    {
        perror("write_to_fd_from_buffer");
        exit(EXIT_FAILURE);
    }

    move_left_n(source, result_write);
}

void read_to_buffer_from_fd(buffer_t* destination, int source)
{
    int result_read;
    
    int free_space = get_length_free_space_in_buffer(destination);
    char* pointer = get_last_position_in_buffer(destination);

    if (!free_space)
    {
        perror("need increase buffer size");
        exit(EXIT_FAILURE);
    }

    result_read = read(source, pointer, free_space);

    if (result_read == -1)
    {
        perror("read_to_buffer_from_fd");
        exit(EXIT_FAILURE);
    }

    update_buffer_length(destination, result_read);
}

/*
+ 1 gives guarantee extract string separator. If we have string:"\n" then end_line = 0, 
and we can use contruction dest[end_line] = 0, which present correct string
*/
char* get_string(buffer_t* buffer, char string_separator)
{
    int end_string = find_string(buffer, string_separator);
    char* dest;

    if (end_string == -1)
        return NULL;

    dest = get_mem(end_string + 1);

    memmove(dest, buffer->buffer, end_string + 1);

    dest[end_string] = 0;

    move_left_n(buffer, end_string + 1);

    return dest;
}

void set_string(buffer_t* buffer, const char* str)
{
    size_t size;

    size = strlen(str);

    clear_buffer(buffer);

    if (str[size-1] != 0)
    {
        perror("set_string: not string");
        return;
    }
    if (size > BUFFER_SIZE)
    {
        perror("set_string: increase BUFFER_SIZE");
        return;
    }

    strcpy(buffer->buffer, str);
    buffer->length = size;
}


void clear_buffer(buffer_t* buffer)
{
    buffer->length = 0;
}

int is_have_info(buffer_t* buffer)
{
    return buffer->length;
}




static void move_left_n(buffer_t* buffer, int offset)
{
    int n;
    char* dest;

    if (offset <= 0)
        return;    

    n = buffer->length - offset;
    dest = get_mem(BUFFER_SIZE);

    memmove(dest, buffer->buffer + offset, n);

    free(buffer->buffer);

    buffer->buffer = dest;
    buffer->length = n;
}

static char* get_mem(size_t size)
{
    char* new_buffer;

    new_buffer = malloc(size);

    if (!new_buffer)
    {
        perror("get_mem");
        exit(EXIT_FAILURE);
    }

    return new_buffer;
}


static int get_length_free_space_in_buffer(buffer_t* buffer)
{
    return BUFFER_SIZE - buffer->length;
}

static char* get_last_position_in_buffer(buffer_t* buffer)
{
    return buffer->buffer + buffer->length;
}
/*work with positive or negative value*/
static void update_buffer_length(buffer_t* buffer, int length_change)
{
    buffer->length += length_change;
}

static int find_string(buffer_t* buffer, char string_separator)
{
    int end_string = -1;
    int i = 0;
    for (; i < buffer->length; i++)
    {
        if (buffer->buffer[i] == string_separator)
        {
            end_string = i;
            break;
        }
    }

    return end_string;
}