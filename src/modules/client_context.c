#include "client_context.h"

client_context_t* create_client_context()
{
    client_context_t* client_context;

    client_context = get_mem(sizeof(client_context_t));

    client_context->input_buffer = create_buffer(BASE_BUFFER_SIZE);
    client_context->server_buffer = create_buffer(BASE_BUFFER_SIZE);
    client_context->output_buffer = create_buffer(BASE_BUFFER_SIZE);

    return client_context;
}

void free_client_context(client_context_t* client_context)
{
    free_buffer(client_context->input_buffer);
    free_buffer(client_context->server_buffer);
    free_buffer(client_context->output_buffer);

    free(client_context);
}

void change_client_state(client_context_t* client)
{
    switch(client->state)
    {
        case ready_receive_info_from_server:
            /*if (is_have_string(client->server_buffer) != -1)*//*TODO:*/
                client->state = ready_showing_info_for_client;
        break;
        case ready_receive_info_from_client:
            /*if (is_have_string(client->input_buffer) != -1)*/
                client->state = ready_send_info_to_server;
        break;
        case ready_showing_info_for_client:
            if (is_buffer_empty(client->server_buffer))
                client->state = ready_receive_info_from_client;
        break;
        case ready_send_info_to_server:
            if (is_buffer_empty(client->input_buffer))
                client->state = ready_receive_info_from_server;
        break;
        default:
        break;
    }
}
