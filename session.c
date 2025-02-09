#include "session.h"


session_t* create_session(int connected_fd, const char* msg)
{
    session_t* session;

    session = get_mem(sizeof(session_t));

    session->socket_fd = connected_fd;
    session->current_state = welcome_state;
    session->status_state = ready_send_info;

    session->buffer = create_buffer();

    copy_string_to_buffer(session->buffer, msg);

    return session;
}

void free_session(session_t* session)
{
    free_buffer(session->buffer);
    free(session);
}



/*
int need_write(struct session_t* session_t)
{
    if(session_t == NULL)
        return -1;
    return (*session_t).size_write_buff;*//*number symbols which need write
    return -1;
}

int received_info_for_write(struct session_t* session_t)
{
    return -1; session_t).rcvd_write;
}

void reset_received_info_for_write(struct session_t* session_t)
{
    (*session_t).rcvd_write = 0;
}

void set_write_info(struct session_t* session_t, const char* msg)
{
    strcpy((*session_t).buff_write, msg);

    (*session_t).size_write_buff = strlen((*session_t).buff_write);

    (*session_t).rcvd_write = 1;
}
*/