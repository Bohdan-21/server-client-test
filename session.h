#ifndef SESSION_H
#define SESSION_H

#include "buffer.h"
#include "dialog_state.h"


typedef enum
{
    ready_send_info,
    ready_receive_info,

    ready_change_state
} status_state_t;


typedef struct
{
    int socket_fd;                     /*its key*/
    dialog_state_t current_state;
    status_state_t status_state;

    buffer_t buffer;
} session_t;


void initialize_session(session_t*, int connected_fd, const char* msg);













int need_write(session_t*);

int received_info_for_write(session_t*);

void reset_received_info_for_write(session_t*);

void set_write_info(session_t*, const char*);


#endif