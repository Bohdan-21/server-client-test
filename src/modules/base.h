#ifndef BASE_H
#define BASE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "memory_manager.h"

#define C_STRING_SEPARATOR '\0'
#define DIRTY_STRING_SEPARATOR '\n'

/*
void* get_mem(size_t size);
*/

char* make_new_copy_string(const char* str, int length);

size_t make_copy_string(char* dest, const char* src);

void replace_symbol(char* str, int length, char pattern, char replace);


#endif