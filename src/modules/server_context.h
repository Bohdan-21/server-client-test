#include "list.h"

typedef struct 
{
    int max_d;
    int listen_socket_fd;
    list_t* dialogs;
    list_t* sessions;
} server_context_t;