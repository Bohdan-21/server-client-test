#include "session.h"


static void update_session_data(session_t* session, int current_dialog_id, const char* msg);



session_t* create_session(int connected_fd, int current_dialog_id, const char* msg)
{
    session_t* session;

    session = get_mem(sizeof(session_t));

    session->socket_fd = connected_fd;
    session->buffer = create_buffer();
    
    update_session_data(session, current_dialog_id, msg);

    return session;
}

void free_session(session_t* session)
{
    free_buffer(session->buffer);
    free(session);
}



int get_session_fd(session_t* session)
{
    return session->socket_fd;
}

int get_session_current_dialog_id(session_t* session)
{
    return session->current_dialog_id;
}

session_state_t get_session_state(session_t* session)
{
    return session->state;
}

buffer_t* get_session_buffer(session_t* session)
{
    return session->buffer;
}



void update_session_current_dialog_id(session_t* session, int new_dialog_id)
{
    session->current_dialog_id = new_dialog_id;
}



static void update_session_data(session_t* session, int current_dialog_id, const char* msg)
{
    session->current_dialog_id = current_dialog_id;
    session->state = ready_send_info;
    copy_string_to_buffer(session->buffer, msg);
}



void try_change_session_state(session_t* session, int current_dialog_id, 
                              const char* msg)
{
    if (session->state == ready_send_info)
    {
        if (is_buffer_empty(session->buffer))
            session->state = ready_receive_info;
    }
    else if (session->state == ready_receive_info)
    {
        if (is_have_string(session->buffer) != -1)
            update_session_data(session, current_dialog_id, msg);
    }
}

