#include "session.h"


static void update_session_data(session_t* session, const dialog_t* dialog);



session_t* create_session(int connected_fd, const dialog_t* new_dialog, int count_answer)
{
    session_t* session;

    session = get_mem(sizeof(session_t));

    session->socket_fd = connected_fd;
    session->buffer = create_buffer();

    /**/
    session->connection_state = connection_success;
    session->answers = get_mem(sizeof(char*)* count_answer);
    session->count_answer = count_answer;
    /**/
    
    update_session_data(session, new_dialog);

    return session;
}

void free_session(void* data)
{
    session_t* session = (session_t*)data;

    for (int i = 0; i < session->count_answer;i++)
    {
        /*UB!!!!!!!!!!*/
        free(session->answers[i]);
        /*need fix this what if session is not ended correct and in the answer store rubish*/
        /*have only 2 answer then like we need 3*/
        /*UB!!!!!!!*/
    }
    free(session->answers);

    free_buffer(session->buffer);
    free(session);
}




void update_session_current_dialog_id(session_t* session, const dialog_t* dialog)
{
    session->dialog = dialog;
}

void try_change_session_state(session_t* session, const dialog_t* dialog)
{
    update_session_data(session, dialog);
}



static void update_session_data(session_t* session, const dialog_t* dialog)
{
    session->dialog = dialog;
    session->state = ready_send_info;
    /*copy_string_to_buffer(session->buffer, msg);*//*TODO:*/
    
    int length = make_copy_string(session->buffer->ptr, dialog->msg);
    session->buffer->size = length;
}