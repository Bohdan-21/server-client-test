#ifndef LOGI_H
#define LOGI_H

#include <fcntl.h>

#include "base.h"
#include "session.h"


#define LOG_FILE_PATH "./server.log"
#define START_SERVER_MESSAGE "\n\nSERVER STARTED\n"


typedef struct
{
    char* buffer;
    int size;

    int log_file_fd;

    char* current_position;
    int length;/*length all data which store on buffer*/
}log_data_t;



log_data_t* initialize_logging_module(int buffer_size);

void free_loging_module(void* data);



int is_logging(log_data_t* log_data);

void push_data(log_data_t* log_data, const session_t* session);

void send_log_data(log_data_t* log_data);

#endif