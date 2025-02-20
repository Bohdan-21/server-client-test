#ifndef BASE_H
#define BASE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define STRING_SEPARATOR '\n'


void* get_mem(size_t size);

/*return position where find separator or -1*/
int is_have_separator(const char* str, int length);

/*worked with "dirty" string, where separator is '\n' not '\0'*/
char* make_copy_string(const char* str);

/*worked with "dirty" string, where separator is '\n' not '\0'*/
char* make_c_string(char* str);

#endif