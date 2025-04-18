#ifndef BUFFER_H
#define BUFFER_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "base.h"


#define BASE_SIZE 128


typedef struct 
{
    char* ptr;
    size_t size;
} buffer_t;


buffer_t* create_buffer();

void free_buffer(void*);



char* get_buffer_start_pointer(buffer_t* buffer);

char* get_buffer_end_pointer(buffer_t* buffer);

size_t get_buffer_free_space(buffer_t* buffer);

size_t get_buffer_content_length(buffer_t* buffer);



void clear_buffer(buffer_t* buffer);

int is_buffer_empty(buffer_t* buffer);
/*return position where find separator or -1*/
int find(buffer_t* buffer, char separator);



void move_content_left(buffer_t* buffer, int count_move);


#endif