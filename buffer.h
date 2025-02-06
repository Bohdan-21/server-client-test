#ifndef BUFFER_H
#define BUFFER_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 128
#define END_LINE_SYMBOL '\n'

typedef struct
{
    char* buffer;
    int length;
    
} buffer_t;

void initialize_buffer(buffer_t* buffer);

void free_buffer(buffer_t* buffer);


void write_from_buffer_to_fd(int destination, buffer_t* source);

void read_from_fd_to_buffer(buffer_t* destination, int source);

char* get_string(buffer_t* buffer, char string_separator);

void clear_buffer(buffer_t* buffer);

int is_have_info(buffer_t* buffer);

#endif