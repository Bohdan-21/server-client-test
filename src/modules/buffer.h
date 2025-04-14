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

int find(buffer_t* buffer, char separator);



void move_content_left(buffer_t* buffer, int count_move);


#endif






// #define BUFFER_SIZE 128

// typedef struct
// {
//     char* buffer;
//     size_t length;
    
// } buffer_t;


// buffer_t* create_buffer();

// void free_buffer(buffer_t* buffer);




// void update_buffer_content_length(buffer_t* buffer, int increase_content_length);



// void removal_content_left_n_buffer(buffer_t* buffer, int removal);




// /*
// buffer cant read and write, buffer must manage struct data
// read and write this is task for server
// remove this
// *//*
// void write_to_fd_from_buffer(int destination, buffer_t* source);

// void read_to_buffer_from_fd(buffer_t* destination, int source);
// */
// /*
// remove this
// */

// char* get_string(buffer_t* buffer);

// void copy_string_to_buffer(buffer_t* dest, const char* src);



// void clear_buffer(buffer_t* buffer);

// int is_buffer_empty(buffer_t* buffer);

// int is_have_string(buffer_t* buffer);

// #endif