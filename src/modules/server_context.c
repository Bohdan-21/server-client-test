#include "server_context.h"

server_context_t* create_server_context()
{
    server_context_t* server_context = get_mem(sizeof(server_context_t));
    
    server_context->dialogs = create_list();
    server_context->sessions = create_list();

    return server_context;
}

void free_server_context(server_context_t* server_context)
{
    free_list(server_context->sessions, NULL);
    free_list(server_context->dialogs, NULL);

    free(server_context);

    server_context = NULL;
}