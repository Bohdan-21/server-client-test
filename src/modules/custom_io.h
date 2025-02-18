#ifndef CUSTOM_IO_H
#define CUSTOM_IO_H

#include <unistd.h>

#include "buffer.h"




int read_from_fd(buffer_t* destination, int source);

int write_to_fd(int destination, buffer_t* source);

#endif