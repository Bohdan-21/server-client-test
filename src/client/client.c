#include <fcntl.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <time.h>
#include <signal.h>
#include <termios.h>

#include "../modules/buffer.h"
#include "../modules/custom_io.h"
#include "../modules/client_context.h"

#define SOCKET_PATH "./socket_file"


#define NOTHING_DO 0
#define STOP_CLIENT 1

#define WAITING_TIME_IN_SEC 5
#define WAITING_TIME_IN_NSEC 0


#define STANDART_INPUT 0    /*stdin*/
#define STANDART_OUTPUT 1   /*stdout*/




volatile sig_atomic_t client_programm_state = NOTHING_DO;



static void signal_handler(int);



static int connect_to_server(client_context_t* client_context);

static int set_nonblock_mode_fd(int fd);



static void setup_signal(sigset_t* mask, sigset_t* oldmask);

static void setup_signal_mask(sigset_t* mask, sigset_t* oldmask);



static void setup_timeout(client_context_t* client_context);

static void prepare_fd_sets(client_context_t* client_context);


static int event_handler(client_context_t* client_context);

static int handle_read(client_context_t* client_context);

static int handle_write(client_context_t* client_context);




static int start_client(client_context_t** client_context);

static int initialize_client_context(client_context_t* client_context);

static int setup_server_fd(client_context_t* client_context);

static int initialize_input_fd(client_context_t* client_context);

static int initialize_output_fd(client_context_t* client_context);



static void stop_client(client_context_t* client_context);

static void close_connection(int fd);



static void shutdown_client(int exit_status, client_context_t* client_context);


/*-1 - fault, 0 - success*/
static int start_client(client_context_t** client_context)
{
    *client_context = create_client_context();

    if (initialize_client_context(*client_context) == -1)
        return -1;

    (*client_context)->state = receive_from_server;
    (*client_context)->output_state = not_yet_send;

    return 0;
}
/*0 - success, -1 - fault*/
static int initialize_client_context(client_context_t* client_context)
{    
    if (connect_to_server(client_context))
        return -1;

    client_context->max_d = client_context->socket_fd;

    if (setup_server_fd(client_context) == -1)
        return -1;

    if (initialize_input_fd(client_context) == -1)
        return -1;
    
    if (initialize_output_fd(client_context) == -1)
        return -1;

    return 0;
}
/*-1 if cant connect*/
static int connect_to_server(client_context_t* client_context)
{
    int result_connection;
    struct sockaddr_un addr;

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    client_context->socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    
    if (client_context->socket_fd == -1)
        return -1;

    result_connection = connect(client_context->socket_fd, (struct sockaddr*)&addr, 
                                sizeof(struct sockaddr_un));

    return result_connection;
}

static int setup_server_fd(client_context_t* client_context)
{
    return set_nonblock_mode_fd(client_context->socket_fd);
}

static int initialize_input_fd(client_context_t* client_context)
{
    client_context->input_fd = STANDART_INPUT;
    return set_nonblock_mode_fd(STANDART_INPUT);
}

static int initialize_output_fd(client_context_t* client_context)
{
    client_context->output_fd = STANDART_OUTPUT;
     return set_nonblock_mode_fd(STANDART_OUTPUT);
}

static int set_nonblock_mode_fd(int fd)
{
    int flags;

    flags = fcntl(fd, F_GETFL);

    if (flags == -1)
        return -1;

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
        return -1;
    
    return 0;
}



static void stop_client(client_context_t* client_context)
{
    if (client_context)
    {
        /*release resource from input and output*/
        close_connection(client_context->socket_fd);
        free_client_context(client_context);
    }
}

static void close_connection(int fd)
{
    shutdown(fd, SHUT_RDWR);
    close(fd);
}



static void shutdown_client(int exit_status, client_context_t* client_context)
{
    stop_client(client_context);
    exit(exit_status);
}


static void data_proccessing(client_context_t* client_context);



int main()
{
    int result;
    int result_handle_event;
    sigset_t mask, oldmask;
    client_context_t* client_context;

    setup_signal(&mask, &oldmask);


    if (start_client(&client_context) == -1)
        shutdown_client(EXIT_FAILURE, client_context);

    for (;;)
    {
        /*event selection*/
        setup_timeout(client_context);
        prepare_fd_sets(client_context);
        /*event selection*/
        
        result = pselect(client_context->max_d + 1, &client_context->read_fds, 
                         &client_context->write_fds, NULL, 
                         &client_context->timeout, &oldmask);

        /*event processing*/
        if (result == -1)
        {
            if (client_programm_state == STOP_CLIENT)
                shutdown_client(EXIT_SUCCESS, client_context);
        }
        else if (result == 0)
        {
            /*time is out*/
        }
        else if (result > 0)
        {
            /*1 - handle event*//*if something goes wrong just shutdown_client*/
            /*2 - data processing*/

            if (event_handler(client_context) == -1)
                shutdown_client(EXIT_FAILURE, client_context);

            data_proccessing(client_context);
        }
        /*event processing*/
    }

    exit(EXIT_SUCCESS);
}



static void signal_handler(int signum)
{
    signal(signum, signal_handler);

    if (client_programm_state == NOTHING_DO)
    {
        if (signum == SIGINT || signum == SIGPIPE)/*server call shutdown*/
            client_programm_state = STOP_CLIENT;        
    }
}




static void setup_signal(sigset_t* mask, sigset_t* oldmask)
{
    signal(SIGINT, signal_handler);
    signal(SIGPIPE, signal_handler);
    setup_signal_mask(mask, oldmask);
}

static void setup_signal_mask(sigset_t* mask, sigset_t* oldmask)
{
    sigemptyset(mask);
    sigaddset(mask, SIGINT);
    sigaddset(mask, SIGPIPE);
    sigprocmask(SIG_BLOCK, mask, oldmask);
}



static void setup_timeout(client_context_t* client_context)
{
    client_context->timeout.tv_sec = WAITING_TIME_IN_SEC;
    client_context->timeout.tv_nsec = WAITING_TIME_IN_NSEC;
}

static void prepare_fd_sets(client_context_t* client_context)
{
    FD_ZERO(&client_context->read_fds);
    FD_ZERO(&client_context->write_fds);

    if (client_context->state == receive_from_server)
    {
        /*we can receive event from server every second*/
        /*if server down/restart or send data we receive event to*/
        /*but in real case we know this when try read, thats why if here*/
        /*save resource on tracking*/
        if (is_buffer_empty(client_context->output_buffer))
            FD_SET(client_context->socket_fd, &client_context->read_fds);
        else /*if (!is_buffer_empty(client_context->output_buffer))*/
            FD_SET(client_context->output_fd, &client_context->write_fds);
    }
    else /*receive_from_user*/ 
    {
        /*we need receive data from user*/
        if (is_buffer_empty(client_context->output_buffer))
            FD_SET(client_context->input_fd, &client_context->read_fds);
        else /*we alreay received data from user, and send them to server*/
            FD_SET(client_context->socket_fd, &client_context->write_fds);
    }





    /*least resistance*//*
    if (client_context->state == ready_receive_info_from_server)
        FD_SET(client_context->socket_fd, read_fds);
    else if (client_context->state == ready_receive_info_from_client)
        FD_SET(client_context->input_fd, read_fds);
    else if (client_context->state == ready_showing_info_for_client)
        FD_SET(client_context->output_fd, write_fds);
    else if (client_context->state == ready_send_info_to_server)
        FD_SET(client_context->socket_fd, write_fds);*/
    /*least resistance*/
}


/*return 0 or -1*/
static int event_handler(client_context_t* client_context)
{
    int result;

    if (client_context->state == receive_from_server)
    {
        if (is_buffer_empty(client_context->output_buffer))
            result = handle_read(client_context);
        else 
            result = handle_write(client_context);
    }
    else if (client_context->state == receive_from_user)
    {
        if (is_buffer_empty(client_context->output_buffer))
            result = handle_read(client_context);
        else
            result = handle_write(client_context);
    }
    
    return result;

    /*int result;

    if (client_context->state == ready_receive_info_from_server ||
        client_context->state == ready_receive_info_from_client)*//*read*/
/*
        result = handle_read(client_context, &client_context->read_fds);


    else *//*if (client_context->state == ready_showing_info_for_client ||
             client_context->state == ready_send_info_to_server)*//*write*/
    
      /*  result = handle_write(client_context, &client_context->write_fds);

    change_client_state(client_context);


    if (result == -1 || result == 0)
        return -1;

    return 0;*/
}

static int handle_read(client_context_t* client_context)
{
    int result = -1;

    if (FD_ISSET(client_context->socket_fd, &client_context->read_fds))
        result = read_from_fd(client_context->server_buffer, client_context->socket_fd);
    else if (FD_ISSET(client_context->input_fd, &client_context->read_fds))
        result = read_from_fd(client_context->input_buffer, client_context->input_fd);

    if (result == 0)/*if count readed symbol 0 that mean server close connection*/
        return -1;

    return result;

    /*int result = -1;

    if (FD_ISSET(client_context->input_fd, &client_context->read_fds))*//*input data from terminal*/
    /*{
        result = read_from_fd(client_context->input_buffer, client_context->input_fd);
        printf("rc i\n");
        replace_symbol(client_context->input_buffer->ptr, client_context->input_buffer->size, DIRTY_STRING_SEPARATOR, C_STRING_SEPARATOR);
    }
    else if (FD_ISSET(client_context->socket_fd, &client_context->read_fds))
    {
        result = read_from_fd(client_context->server_buffer, client_context->socket_fd);
    }

    return result;*/
}

static int handle_write(client_context_t* client_context)
{
    int result = -1;

    if (FD_ISSET(client_context->socket_fd, &client_context->write_fds))
        result = write_to_fd(client_context->socket_fd, client_context->output_buffer);
    else if ((FD_ISSET(client_context->output_fd, &client_context->write_fds)))
        result = write_to_fd(client_context->output_fd, client_context->output_buffer);

    return result;


    /*int result = -1;

    if (FD_ISSET(client_context->output_fd, &client_context->write_fds))
    {
        result = write_to_fd(client_context->output_fd, client_context->server_buffer);
        replace_symbol(client_context->server_buffer->ptr, client_context->server_buffer->size, C_STRING_SEPARATOR, DIRTY_STRING_SEPARATOR);
    }
    else if (FD_ISSET(client_context->socket_fd, &client_context->write_fds))
    {
        result = write_to_fd(client_context->socket_fd, client_context->input_buffer);
    }
    
    return result;*/
}






static void data_proccessing(client_context_t* client_context)
{
    




    /*
    if (!is_buffer_empty(client_context->output_buffer))
        return;

    if (client_context->state == receive_from_server && 
        find(client_context->server_buffer, C_STRING_SEPARATOR) != -1)
    {

        push_string_on_buffer(client_context->output_buffer, 
                              client_context->server_buffer->ptr);

        clear_buffer(client_context->server_buffer);

        replace_symbol(client_context->output_buffer->ptr, 
                       client_context->output_buffer->length,
                       C_STRING_SEPARATOR, DIRTY_STRING_SEPARATOR);
    }
    else if (client_context->state == receive_from_user &&
             find(client_context->input_buffer, DIRTY_STRING_SEPARATOR) != -1)
    {
        replace_symbol(client_context->input_buffer->ptr, 
                       client_context->input_buffer->length, 
                       DIRTY_STRING_SEPARATOR, C_STRING_SEPARATOR);

        push_string_on_buffer(client_context->output_buffer,
                              client_context->input_buffer->ptr);

        clear_buffer(client_context->input_buffer);
    }*/
}


