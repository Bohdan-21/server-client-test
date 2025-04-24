#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H


#include <time.h>
#include <sys/select.h>
#include <sys/socket.h>

#include "base.h"
#include "buffer.h"


typedef enum
{
    ready_send,
    waiting_send
}output_state_t;


typedef enum 
{
    receive_from_user,
    receive_from_server,




    ready_receive_info_from_server,/*get data from server*/
    ready_showing_info_for_client,/*print geted data from server*/
    ready_receive_info_from_client,/*get data from client*/
    ready_send_info_to_server/*send geted data from client to server*/
} client_state_t;

typedef struct 
{
    int max_d;

    int input_fd;
    int output_fd;
    int socket_fd;

    client_state_t state;
    output_state_t output_state;

    buffer_t* input_buffer;/*taking data only from input*/
    buffer_t* server_buffer;/*taking data only from server/socket*/
    buffer_t* output_buffer;/*send data on output and server/socket*/

    struct timespec timeout;
    fd_set read_fds, write_fds;

} client_context_t;


client_context_t* create_client_context();

void free_client_context(client_context_t* client_connection);

void change_client_state(client_context_t* client);


#endif