#ifndef BUFFER_H
#define BUFFER_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "base.h"

#define BUFFER_SIZE 128
#define STRING_SEPARATOR '\n'

typedef struct
{
    char* buffer;
    int length;
    
} buffer_t;

buffer_t* create_buffer();

void free_buffer(buffer_t* buffer);


/*
buffer cant read and write, buffer must manage struct data
read and write this is task for server
remove this
*/
void write_to_fd_from_buffer(int destination, buffer_t* source);

void read_to_buffer_from_fd(buffer_t* destination, int source);
/*
remove this
*/

char* get_string(buffer_t* buffer);

void copy_string_to_buffer(buffer_t* dest, const char* src);

int find_string(buffer_t* buffer);


void clear_buffer(buffer_t* buffer);

int is_buffer_empty(buffer_t* buffer);

#endif