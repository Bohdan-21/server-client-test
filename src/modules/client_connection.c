#include "client_connection.h"




client_connection_t* create_client_connection(int input_fd, int output_fd, 
                                              int socket_fd)
{
    client_connection_t* client_connection;

    client_connection = get_mem(sizeof(client_connection_t));

    client_connection->input_fd = input_fd;
    client_connection->output_fd = output_fd;
    client_connection->socket_fd = socket_fd;

    client_connection->client_state = ready_receive_info_from_server;

    client_connection->input_buffer = create_buffer();
    client_connection->server_buffer = create_buffer();

    return client_connection;
}

void close_client_connection(client_connection_t* client_connection)
{
    shutdown(client_connection->socket_fd, SHUT_RDWR);
    close(client_connection->socket_fd);
    close(client_connection->input_fd);
    close(client_connection->output_fd);
}

void free_client_connection(client_connection_t* client_connection)
{
    free_buffer(client_connection->server_buffer);
    free_buffer(client_connection->input_buffer);

    free(client_connection);
}