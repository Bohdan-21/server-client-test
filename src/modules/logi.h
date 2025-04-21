#ifndef LOGI_H
#define LOGI_H

#include <fcntl.h>

#include "base.h"
#include "buffer.h"
#include "session.h"
#include "custom_io.h"


#define LOG_FILE_PATH "./server.log"

#define START_SERVER_MESSAGE "\n\nSERVER STARTED\n"

#define MARK_START_SESSION "Start logging ended session"
#define MARK_END_SESSION "End logging ended session\n"


typedef struct
{
    int log_file_fd;

    buffer_t* buffer;
}log_data_t;



log_data_t* initialize_logging_module(size_t buffer_size);

void free_loging_module(void* data);



int is_logging(log_data_t* log_data);

void push_data(log_data_t* log_data, const session_t* session);

int send_log_data(log_data_t* log_data);

#endif