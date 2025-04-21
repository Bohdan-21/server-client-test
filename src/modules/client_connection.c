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

    client_connection->input_buffer = create_buffer(BASE_BUFFER_SIZE);
    client_connection->server_buffer = create_buffer(BASE_BUFFER_SIZE);

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

void change_client_state(client_connection_t* client)
{
    switch(client->client_state)
    {
        case ready_receive_info_from_server:
            /*if (is_have_string(client->server_buffer) != -1)*//*TODO:*/
                client->client_state = ready_showing_info_for_client;
        break;
        case ready_receive_info_from_client:
            /*if (is_have_string(client->input_buffer) != -1)*/
                client->client_state = ready_send_info_to_server;
        break;
        case ready_showing_info_for_client:
            if (is_buffer_empty(client->server_buffer))
                client->client_state = ready_receive_info_from_client;
        break;
        case ready_send_info_to_server:
            if (is_buffer_empty(client->input_buffer))
                client->client_state = ready_receive_info_from_server;
        break;
    }
}
