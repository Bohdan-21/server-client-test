#ifndef SESSION_H
#define SESSION_H

#include "buffer.h"
#include "custom_io.h"
#include "dialog.h"


typedef enum
{
    ready_send_info,
    ready_receive_info
} session_state_t;

typedef enum
{
    connection_success,
    connection_log,
    connection_end,
    connection_drop
} connection_state_t;

typedef struct
{
    int socket_fd;                     /*its key*/
    
    session_state_t state;
    connection_state_t connection_state;

    const dialog_t* dialog;
    buffer_t* buffer;
    
    char** answers;
    int number_current_answer;
} session_t;


session_t* create_session(int connected_fd, const dialog_t* new_dialog, int count_answer);

void free_session(void*);



int extract_answer(session_t* session);

void update_session_data(session_t* session, const dialog_t* dialog);


#endif