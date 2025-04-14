#ifndef BASE_H
#define BASE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define STRING_SEPARATOR '\n'


void* get_mem(size_t size);

char* make_copy_string(const char* str, int length);

char* make_c_string(char* str, int modify_position);

#endif