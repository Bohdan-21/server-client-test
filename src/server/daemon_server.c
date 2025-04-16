/*
SIGUSR1 - reload config
SIGINT - stop server
*/

#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <string.h>
#include <sys/select.h>
#include <fcntl.h>

#include "../modules/list.h"
#include "../modules/buffer.h"
#include "../modules/dialog.h"
#include "../modules/dialog_state.h"
#include "../modules/custom_io.h"
#include "../modules/session.h"

#include "../modules/server_context.h"


#define NOTHING_DO 0
#define RELOAD_CONFIG 1
#define STOP_SERVER 2

#define WAITING_TIME_IN_SEC 5
#define WAITING_TIME_IN_NSEC 0

#define DEFAULT_DIALOG_ID 1
#define ERROR_DIALOG_ID -1

#define SOCKET_PATH "./socket_file"
#define CONFIG_PATH "./config/dialog.txt"

#define DEBUG
#define GET_PROCESS_ID_WHEN_START


enum 
{
    queue_connection = 5
};


volatile sig_atomic_t server_state = NOTHING_DO;



void signal_handler(int);



void setup_signal(sigset_t* mask, sigset_t* oldmask);

void setup_signal_mask(sigset_t* mask, sigset_t* oldmask);



static int start_server(server_context_t**);

static int load_config(server_context_t* server_context);

static int initialize_config(server_context_t* server_context, int file_fd);

static char* get_string(buffer_t* buffer);



static int initialize_listen_socket(server_context_t* server_context);



static void stop_server(server_context_t*);

void close_listen_socket(int);

static void close_all_connection(list_t* sessions);



void setup_timeout(server_context_t* server_context);

void prepare_fd_sets(server_context_t* server_context);



static int is_new_connection(server_context_t* server_context);



static void accept_connection(server_context_t* server_context);

static int try_set_nonblock_mode_for_socket(int connection_fd);

static void create_connection_session(server_context_t* server_context, int connection_fd);



static void event_handler(server_context_t* server_context);

static void handle_write_event(session_t* session);

static void handle_read_event(session_t* session);

static void change_session_state(list_t* dialogs, session_t* session);

static const dialog_t* get_dialog(list_t* dialogs, int dialog_id);

static void prepare_session_for_close(session_t* session);


static void remove_ended_session(list_t* sessions);



static void close_connection(int connected_fd);




int main()
{
    int result;
       
    sigset_t mask, oldmask;
    server_context_t* server_context = NULL;



#ifdef GET_PROCESS_ID_WHEN_START
    printf("%d\n", getpid());
#endif



    setup_signal(&mask, &oldmask);

    start_server(&server_context);

    for (;;)
    {
        /*event selection*/
        setup_timeout(server_context);
        prepare_fd_sets(server_context);
        
        /*event selection*/

        result = pselect(server_context->max_d + 1, 
                         &server_context->read_fds, &server_context->write_fds,
                         NULL, &server_context->timeout, &oldmask);

        /*event processing*/

        if (result == -1)/*receive signal*/
        {
            if (server_state == STOP_SERVER)
            {
                stop_server(server_context);
                break;
            }
            else if (server_state == RELOAD_CONFIG)
            {
                stop_server(server_context);
                start_server(&server_context);
                server_state = NOTHING_DO;
            }
        }
        else if (result == 0)
        {
            printf("Time out\n");
        }
        else if (result > 0)
        {
            /*printf("Some fd ready");*/
            if (is_new_connection(server_context))
            {
                accept_connection(server_context);
                    
                result--;
            }

            if (result > 0)
                event_handler(server_context);
        }

        remove_ended_session(server_context->sessions);
        /*event processing*/
    }

    exit(EXIT_SUCCESS);
}



void signal_handler(int signum)
{
    signal(signum, signal_handler);

    if (server_state == NOTHING_DO)
    {
        if (signum == SIGINT)
            server_state = STOP_SERVER;
        else if (signum == SIGUSR1)
            server_state = RELOAD_CONFIG;
    }
}



void setup_signal(sigset_t* mask, sigset_t* oldmask)
{
    signal(SIGINT, signal_handler);
    signal(SIGUSR1, signal_handler);

    setup_signal_mask(mask, oldmask);
}

void setup_signal_mask(sigset_t* mask, sigset_t* oldmask)
{
    sigemptyset(mask);
    sigaddset(mask, SIGINT);
    sigaddset(mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, mask, oldmask);
}



static int start_server(server_context_t** server_context)
{
    *server_context = create_server_context();

    if (load_config(*server_context) == -1)
        return -1;

#ifdef DEBUG
    printf("Config loaded\n");
#endif

    if (initialize_listen_socket(*server_context) == -1)
        return -1;

#ifdef DEBUG
    printf("Socket initialized\n");
#endif

    (*server_context)->max_d = (*server_context)->listen_socket_fd;

    return 0;
}
/*TODO:this need check|return 0 or -1*/
static int load_config(server_context_t* server_context)
{
    int result;
    int config_file_fd = open(CONFIG_PATH, O_RDONLY);

    if (config_file_fd == -1)
        return -1;

    result = initialize_config(server_context, config_file_fd);

    close(config_file_fd);

    return result;
}
/*return 0 or -1*/
static int initialize_config(server_context_t* server_context, int file_fd)
{
    int result_read;
    int dialog_id = DEFAULT_DIALOG_ID;
    
    char* str;
    dialog_t* dialog;
    buffer_t* buffer = create_buffer();

    while((result_read = read_from_fd(buffer, file_fd)))
    {
        if (result_read == -1)
            break;

        replace_symbol(buffer->ptr, buffer->size, DIRTY_STRING_SEPARATOR, C_STRING_SEPARATOR);

        while((str = get_string(buffer)))
        {
            dialog = create_dialog(dialog_id, str);/*maybe need make copy for dialog?*/

            dialog_id++;

            create_node(server_context->dialogs, dialog);

#ifdef DEBUG
            printf("%s\n", str);
#endif
        }
    }
    
    free_buffer(buffer);

    return result_read;
}
/*return NULL or extracted string*/
static char* get_string(buffer_t* buffer)
{
    char* result;
    int position;

    position = find(buffer, C_STRING_SEPARATOR);

    if (position == -1)
        return NULL;

    position += 1;/*including separator*/

    result = make_copy_string(buffer->ptr, position);

    move_content_left(buffer, position);

    return result;
}



static int initialize_listen_socket(server_context_t* server_context)
{
    int result;
    struct sockaddr_un addr;

    server_context->listen_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (server_context->listen_socket_fd == -1)
        return -1;

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    result = bind(server_context->listen_socket_fd, 
                  (struct sockaddr*)&addr, 
                  sizeof(struct sockaddr_un));

    if (result == -1)
        return -1;

    result = listen(server_context->listen_socket_fd, queue_connection);

    return result;
}



static void stop_server(server_context_t* server_context)
{
    remove_all_node(server_context->dialogs, free_dialog);
    /*destroy data*/

    close_listen_socket(server_context->listen_socket_fd);

    close_all_connection(server_context->sessions);
    remove_all_node(server_context->sessions, free_session);    

    free_server_context(server_context); /*to this moment all data must be destroy*/
}

static void close_listen_socket(int listen_fd)
{
    close_connection(listen_fd);
    unlink(SOCKET_PATH);
}

static void close_all_connection(list_t* sessions)
{
    session_t* session;
    node_t* node;

    node = sessions->pointer_in_head;

    for (;node; node = node->next)
    {
        if ((session = (session_t*)node->data))
            close_connection(session->socket_fd);
    }
}



void setup_timeout(server_context_t* server_context)
{
    server_context->timeout.tv_sec = WAITING_TIME_IN_SEC;
    server_context->timeout.tv_nsec = WAITING_TIME_IN_NSEC;
}

void prepare_fd_sets(server_context_t* server_context)
{
    node_t* node;
    session_t* session;

    FD_ZERO(&server_context->read_fds);
    FD_ZERO(&server_context->write_fds);

    FD_SET(server_context->listen_socket_fd, &server_context->read_fds);

    node = server_context->sessions->pointer_in_head;

    for (; node; node = node->next)
    {
        if ((session = (session_t*)node->data))
        {
            if (session->state == ready_send_info)
                FD_SET(session->socket_fd, &server_context->write_fds);
            else if (session->state == ready_receive_info)
                FD_SET(session->socket_fd, &server_context->read_fds);
        }
    }
}



static int is_new_connection(server_context_t* server_context)
{
    return FD_ISSET(server_context->listen_socket_fd, 
                    &server_context->read_fds);
}



/*return fd connected client or -1*/
static void accept_connection(server_context_t* server_context)
{
    int connected_fd;
    int result;

    connected_fd = accept(server_context->listen_socket_fd, NULL, NULL);
    
    if (connected_fd == -1)
        return;

    result = try_set_nonblock_mode_for_socket(connected_fd);

    if (result != -1)
    {
        create_connection_session(server_context, connected_fd);
        
        if (connected_fd > server_context->max_d)
            server_context->max_d = connected_fd;
    }
}
/*return 0 or -1*/
static int try_set_nonblock_mode_for_socket(int connection_fd)
{  
    int flags;
    int result;

    flags = fcntl(connection_fd, F_GETFL);

    if (flags == -1)
    {
        close_connection(connection_fd);
        return -1;
    }

    result = fcntl(connection_fd, F_SETFL, flags | O_NONBLOCK);

    if (result == -1)
    {
        close_connection(connection_fd);
        return -1;
    }
    return 0;
}

static void create_connection_session(server_context_t* server_context, int connection_fd)
{
    session_t* session;
    const dialog_t* dialog;
    int dialog_id = DEFAULT_DIALOG_ID;

    dialog = get_dialog(server_context->dialogs, dialog_id);
    
    session = create_session(connection_fd, dialog_id, dialog->msg);

    create_node(server_context->sessions, (void*)session);
}



static void event_handler(server_context_t* server_context)
{
    node_t* node;
    session_t* session;

    node = server_context->sessions->pointer_in_head;

    for (; node; node = node->next)
    {
        if ((session = (session_t*)node->data))
        {
            if (session->state == ready_send_info)
            {
                if (FD_ISSET(session->socket_fd, &server_context->write_fds))
                    handle_write_event(session);
            }
            else if (session->state == ready_receive_info)
            {
                if (FD_ISSET(session->socket_fd, &server_context->read_fds))
                    handle_read_event(session);
            }

            /*data_processing(session);*//*TODO:add me*/

            change_session_state(server_context->dialogs, session);//this is why client receive message broken pipe, socket close before client send last answer
        }
    }
}

static void handle_write_event(session_t* session)
{
    int result_operation;
    int session_fd;
    buffer_t* buffer;

    session_fd = session->socket_fd;
    buffer = session->buffer;

    result_operation = write_to_fd(session_fd, buffer);

    /*if some reason system call write return -1*/
    if (result_operation == -1)
    {
        prepare_session_for_close(session);
    }
}

static void handle_read_event(session_t* session)
{
    int result_operation;
    int session_fd;
    buffer_t* buffer;

    session_fd = session->socket_fd;
    buffer = session->buffer;

    result_operation = read_from_fd(buffer, session_fd);

    /*
    if we receive ready for read but nothing read (return 0): situation end file, 
                                                              system call shutdown
    or if string length is more BUFFER_SIZE, or if system call return -1
    */
    if (result_operation == -1 || result_operation == 0)
    {
        prepare_session_for_close(session);
    }
}

static void change_session_state(list_t* dialogs, session_t* session)
{
    int dialog_id;
    const dialog_t* dialog;

    dialog_id = session->current_dialog_id + 1;/*next dialog_id*/
    dialog = get_dialog(dialogs, dialog_id);
    
    if (!dialog)
    {
        prepare_session_for_close(session);
        return;
    }
    
    try_change_session_state(session, dialog_id, dialog->msg);
}

static const dialog_t* get_dialog(list_t* dialogs, int dialog_id)
{
    node_t* node;
    dialog_t* dialog;

    node = dialogs->pointer_in_head;

    for (; node; node = node->next)
    {
        if ((dialog = (dialog_t*)node->data))/*maybe cut this*/
        {
            if (dialog->dialog_id == dialog_id)
                return dialog;
        }
    }

    return NULL;
}

static void prepare_session_for_close(session_t* session)
{
    update_session_current_dialog_id(session, ERROR_DIALOG_ID);
}



static void remove_ended_session(list_t* sessions)
{
    int count_id_close = 0;
    node_t* node;
    session_t* session;
    node_t** ids_for_close = get_mem(sizeof(node_t) * sessions->count);

    node = sessions->pointer_in_head;

    for (; node; node = node->next)
    {
        if ((session = (session_t*)node->data))/*maybe remove this*/
        {
            if (session->current_dialog_id == ERROR_DIALOG_ID)
            {
                close_connection(session->socket_fd);

                ids_for_close[count_id_close] = node;
                count_id_close++;
            }
        }
    }

    for (int i = 0; i < count_id_close; i++)
    {
        remove_node(sessions, ids_for_close[i], free_session);
    }

    free(ids_for_close);
}



static void close_connection(int fd)
{
    shutdown(fd, SHUT_RDWR);
    close(fd);
}
