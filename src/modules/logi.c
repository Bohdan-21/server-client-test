#include "logi.h"



log_data_t* initialize_logging_module(int buffer_size)
{
    log_data_t* log_data = get_mem(sizeof(log_data_t));
    
    log_data->buffer = get_mem(buffer_size);
    log_data->current_position = log_data->buffer;

    log_data->size = buffer_size;
    log_data->length = 0;/*no data for log*/

    log_data->log_file_fd = open(LOG_FILE_PATH, O_WRONLY | O_APPEND | O_CREAT, 0644);

    return log_data;
}

void free_loging_module(void* data)
{
    log_data_t* log_data = (log_data_t*)data;

    close(log_data->log_file_fd);
    
    free(log_data->buffer);

    free(log_data);
}



int is_logging(log_data_t* log_data)
{
    return /*log_data->length != 0 && 
          (log_data->current_position - log_data->buffer) == log_data->length;*/0;
          /*replace second condition need more simplest*/
}


void push_data(log_data_t* log_data, const session_t* session)
{
    size_t length;

    for (int i = 0; i < session->count_answer; i++)
    {
        length = make_copy_string(log_data->current_position,
                                  session->answers[i]);

        log_data->current_position += length;
        log_data->length += length;
    }
}
/*changes behavior is_logging, call after setup pselect log_file_fd*/
void send_log_data(log_data_t* log_data)
{
    int length_send_data = log_data->current_position - log_data->buffer;
    length_send_data = log_data->length - length_send_data;

    int result_sending = write(log_data->log_file_fd, log_data->current_position, 
                               length_send_data);

    

    

}