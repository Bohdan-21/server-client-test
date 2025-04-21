#include "logi.h"

log_data_t* initialize_logging_module(size_t buffer_size)
{
    log_data_t* log_data = get_mem(sizeof(log_data_t));
    
    log_data->buffer = create_buffer(buffer_size);
    
    push_string_on_buffer(log_data->buffer, START_SERVER_MESSAGE);

    log_data->log_file_fd = open(LOG_FILE_PATH, O_WRONLY | O_APPEND | O_CREAT, 0644);

    return log_data;
}

void free_logging_module(void* data)
{
    log_data_t* log_data = (log_data_t*)data;

    close(log_data->log_file_fd);
    
    free_buffer(log_data->buffer);

    free(log_data);
}



int is_logging(log_data_t* log_data)
{
    return !is_buffer_empty(log_data->buffer);
}



void push_data(log_data_t* log_data, const session_t* session)
{    
    push_string_on_buffer(log_data->buffer, MARK_START_SESSION);

    for (int i = 0; i < session->number_current_answer; i++)
    {
        push_string_on_buffer(log_data->buffer, session->answers[i]);
    }

    push_string_on_buffer(log_data->buffer, MARK_END_SESSION);

    replace_symbol(log_data->buffer->ptr, log_data->buffer->length,
                   C_STRING_SEPARATOR, DIRTY_STRING_SEPARATOR);
}
/*return -1 or count write byte on log_file*/
int send_log_data(log_data_t* log_data)
{
    return write_to_fd(log_data->log_file_fd, log_data->buffer);
}
