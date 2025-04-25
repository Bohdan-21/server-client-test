#ifndef CLIENT_CONTEXT_H
#define CLIENT_CONTEXT_H

#include <time.h>
#include <sys/select.h>

#include "base.h"
#include "buffer.h"


typedef enum
{
    ready,
    idle
}send_state_t;


typedef enum 
{
    user,
    server
} read_source_t;

typedef struct 
{
    int max_d;

    int input_fd;
    int output_fd;
    int socket_fd;

    read_source_t read_source;
    send_state_t send_state;

    buffer_t* input_buffer;/*receive data*/
    buffer_t* output_buffer;/*send data*/

    struct timespec timeout;
    fd_set read_fds, write_fds;

} client_context_t;


client_context_t* create_client_context();

void free_client_context(client_context_t* client_connection);


#endif