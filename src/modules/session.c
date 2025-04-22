#include "session.h"

session_t* create_session(int connected_fd, const dialog_t* new_dialog, int count_answer)
{
    session_t* session;

    session = get_mem(sizeof(session_t));

    session->socket_fd = connected_fd;
    session->buffer = create_buffer(BASE_BUFFER_SIZE);

    session->connection_state = connection_success;
    session->answers = get_mem(sizeof(char*)* count_answer);
    session->number_current_answer = 0;
    
    update_session_data(session, new_dialog);

    return session;
}

void free_session(void* data)
{
    session_t* session = (session_t*)data;

    for (int i = 0; i < session->number_current_answer;i++)
        free(session->answers[i]);

    free(session->answers);

    free_buffer(session->buffer);
    free(session);
}



int extract_answer(session_t* session)
{
    int position;
    if ((position = find(session->buffer, C_STRING_SEPARATOR)))
    {
        /*+1 for including C_STRING_SEPARATOR*/
        char* tmp = make_new_copy_string(session->buffer->ptr, position + 1);

        session->answers[session->number_current_answer] = tmp;

        session->number_current_answer++;
    }

    return position;
}

void update_session_data(session_t* session, const dialog_t* dialog)
{
    session->dialog = dialog;
    session->state = ready_send_info;
    
    clear_buffer(session->buffer);
    push_string_on_buffer(session->buffer, dialog->msg);
}