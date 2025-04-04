#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H


#include <sys/socket.h>

#include "base.h"
#include "buffer.h"


typedef enum 
{
    ready_receive_info_from_server,/*get data from server*/
    ready_showing_info_for_client,/*print geted data from server*/
    ready_receive_info_from_client,/*get data from client*/
    ready_send_info_to_server/*send geted data from client to server*/
} client_state_t;

typedef struct 
{
    int input_fd;
    int output_fd;
    int socket_fd;

    client_state_t client_state;

    buffer_t* input_buffer;
    buffer_t* server_buffer;

} client_connection_t;


client_connection_t* create_client_connection(int input_fd, int output_fd, 
                                              int socket_fd);

void close_client_connection(client_connection_t* client_connection);

void free_client_connection(client_connection_t* client_connection);

void change_client_state(client_connection_t* client);


#endif