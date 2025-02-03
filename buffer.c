#include "buffer.h"

static void move_left_n(buffer_t* buffer, int n);



static char* get_last_position_for_write(buffer_t*);
static char* get_last_position_for_read(buffer_t*);
static int get_length_free_cell(buffer_t*);
static int get_length_occupied_cell(buffer_t*);
static void update_length(buffer_t*, int);
static void update_offset(buffer_t*, int);

static int find_end_line(char*, int);
static void move_buffer_line_left(buffer_t*, int);

void initialize_buffer(buffer_t* buffer)
{
    buffer->buffer = malloc(BUFFER_SIZE);

    if (!buffer->buffer)
    {
        perror("initialize_buffer");
        exit(EXIT_FAILURE);
    }

    buffer->length = 0;
    buffer->offset = 0;
}

void write_from_buffer_to_fd(int destination, buffer_t* source)
{
    int result_write;

    result_write = write(destination, source->buffer, source->length);

    if (result_write == -1)
    {
        perror("write_from_buffer_to_fd");
        exit(EXIT_FAILURE);
    }

    move_left_n(source, result_write);
}



void read_from_fd_to_buffer(buffer_t* destination, int source)
{
    int result_read;
    int free_cell;
    char* buffer;

    free_cell = get_length_free_cell(destination);
    buffer = get_last_position_for_write(destination);

    if (free_cell == 0)
    {
        perror("need increase buffer size");
        exit(EXIT_FAILURE);
    }

    result_read = read(source, buffer, free_cell);

    if (result_read == -1)
    {
        perror("read");
        exit(EXIT_FAILURE);
    }

    update_length(destination, result_read);
    update_offset(destination, result_read);
}

char* get_line(buffer_t* buffer)
{
    char* dest_string;
    int end_line_position;

    end_line_position = find_end_line(buffer->buffer, buffer->length);

    if (find_end_line == -1)
        return NULL;

    dest_string = malloc(end_line_position);

    if (!dest_string)
    {
        perror("maloc in get_line");
        exit(EXIT_FAILURE);/*return NULL;*/
    }
    
    /*copy without \n*/
    strncpy(dest_string, buffer->buffer, end_line_position - 1);

    /*end_line_position + 1 give next symbol after \n*/
    move_buffer_line_left(buffer, end_line_position + 1);

    return dest_string;
}




static void move_left_n(buffer_t* buffer, int n)
{
    int j = n + 1;
    int n2 = n;

    if (n <= 0)
        return;

    for (int i = 0; i < buffer->length && n != 0; i++, j++, n--)
    {
        if (j >= BUFFER_SIZE)
        {
            buffer->buffer[i] = 0;
            break;
        }
        else
            buffer->buffer[i] = buffer->buffer[j];
    }

    buffer->length -= n2;
}



static char* get_last_position_for_write(buffer_t* buffer)
{
    return buffer->buffer + buffer->offset;
}

static char* get_last_position_for_read(buffer_t*)
{

}


static int get_length_free_cell(buffer_t* buffer)
{
    return BUFFER_SIZE - buffer->length;
}

static int get_length_occupied_cell(buffer_t* buffer)
{
    return buffer->length - buffer->offset;
}

/*work with positive or negative value*/
static void update_length(buffer_t* buffer, int length_change)
{
    buffer->length += length_change;
}

static void update_offset(buffer_t* buffer, int ofsset_change)
{
    buffer->offset += ofsset_change;
}



static int find_end_line(char* data, int length)
{
    int i = 0;
    for (; i < length; i++)
    {
        if (data[i] == END_LINE_SYMBOL)
            break;
    }

    if (i == length)
        return -1;

    return i;
}

static void move_buffer_line_left(buffer_t* buffer, int right_start_position)
{
    for (int i = 0; i < buffer->length; i++, right_start_position++)
    {
        if (right_start_position >= buffer->length)
            buffer->buffer[i] = 0;
        else
            buffer->buffer[i] = buffer->buffer[right_start_position];
    }
}