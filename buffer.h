#ifndef BUFFER_H
#define BUFFER_H

#define BUFFER_SIZE 128

typedef struct
{
    char buffer[BUFFER_SIZE];
    int length;

    int offset;
} buffer_t;




#endif