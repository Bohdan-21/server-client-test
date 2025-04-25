#include "client_context.h"

client_context_t* create_client_context()
{
    client_context_t* client_context;

    client_context = get_mem(sizeof(client_context_t));

    client_context->input_buffer = create_buffer(BASE_BUFFER_SIZE);
    client_context->output_buffer = create_buffer(BASE_BUFFER_SIZE);

    return client_context;
}

void free_client_context(client_context_t* client_context)
{
    free_buffer(client_context->input_buffer);
    free_buffer(client_context->output_buffer);

    free(client_context);
}
