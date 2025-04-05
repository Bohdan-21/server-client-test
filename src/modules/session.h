#ifndef SESSION_H
#define SESSION_H

#include "buffer.h"
#include "dialog_state.h"
#include "custom_io.h"


typedef enum
{
    ready_send_info,
    ready_receive_info,

    ready_change_state
} session_state_t;


typedef struct
{
    int socket_fd;                     /*its key*/
    int current_dialog_id;
    session_state_t state;

    buffer_t* buffer;
} session_t;


session_t* create_session(int connected_fd, int current_dialog_id, const char* msg);

void free_session(void*);



int get_session_fd(session_t* session);

int get_session_current_dialog_id(session_t* session);

session_state_t get_session_state(session_t* session);

buffer_t* get_session_buffer(session_t* session);


void update_session_current_dialog_id(session_t* session, int new_dialog_id);



void try_change_session_state(session_t* session, int current_dialog_id, 
                              const char* msg);


#endif