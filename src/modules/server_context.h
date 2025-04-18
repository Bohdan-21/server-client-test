#ifndef SERVER_CONTEXT_H
#define SERVER_CONTEXT_H

#include "list.h"
#include "logi.h"

typedef struct 
{
    int max_d;
    int listen_socket_fd;

    struct timespec timeout;
    fd_set read_fds, write_fds;

    list_t* dialogs;
    list_t* sessions;

    log_data_t* data_log;
} server_context_t;


server_context_t* create_server_context();

void free_server_context(server_context_t* server_context);


#endif