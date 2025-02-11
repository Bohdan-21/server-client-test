#include "session.h"

session_t* create_session(int connected_fd, const char* msg)
{
    session_t* session;

    session = get_mem(sizeof(session_t));

    session->socket_fd = connected_fd;
    session->dialog_state = welcome_state;
    session->dialog_status = ready_send_info;

    session->buffer = create_buffer();

    copy_string_to_buffer(session->buffer, msg);

    return session;
}

void free_session(session_t* session)
{
    free_buffer(session->buffer);
    free(session);
}



void write_data(session_t* session)
{
    write_to_fd_from_buffer(session->socket_fd, session->buffer);
}

void read_data(session_t* session)
{
    read_to_buffer_from_fd(session->buffer, session->socket_fd);
}


/*
ready_change_status && !ready_change_state --> update: dialog_status
ready_change_status && ready_change_state --> update: dialog_status & dialog_state & 
                                                      buffer
*/
int is_ready_change_session_status(session_t* session)
{
    if (session->dialog_status == ready_send_info)
    {
        if (is_buffer_empty(session->buffer))
            return 1;
    }
    else if (session->dialog_status == ready_receive_info)
    {
        if (find_string(session->buffer))
            return 1;
    }

    return 0;
}

dialog_state_t get_current_dialog_state(session_t* session)
{
    return session->dialog_state;
}

void change_session_status(session_t* session)
{

}
