#ifndef SESSION_H
#define SESSION_H

#include "buffer.h"
#include "dialog_state.h"


typedef enum
{
    ready_send_info,
    ready_receive_info,

    ready_change_state
} dialog_status_t;


typedef struct
{
    int socket_fd;                     /*its key*/
    dialog_state_t dialog_state;
    dialog_status_t dialog_status;

    buffer_t* buffer;
} session_t;


session_t* create_session(int connected_fd, const char* msg);

void free_session(session_t* session);



void write_data(session_t* session);

void read_data(session_t* session);



int is_ready_change_session_status(session_t* session);

dialog_state_t get_current_dialog_state(session_t* session);

void change_session_status(session_t* session);





#endif