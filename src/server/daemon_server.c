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
#define CONFIG_PATH "./config/config.txt"




#define GET_PROCESS_ID_WHEN_START


enum 
{
    queue_connection = 5
};

enum
{
    true = 1,
    false = 0
};



volatile sig_atomic_t server_state = NOTHING_DO;
list_t* session_list;
list_t* dialog_list;


void signal_handler(int);



void setup_signal(sigset_t* mask, sigset_t* oldmask);

void setup_signal_mask(sigset_t* mask, sigset_t* oldmask);



static void start_server(server_context_t*);

static void stop_server(server_context_t*);






void initialize_listen_socket(int*);

void close_listen_socket(int*);






static void load_config();

static void load_config_file(buffer_t* config_data);

static char* get_path_data_file(buffer_t* config_data);

static void initialize_config(buffer_t* config_data, const char* path_to_file);



void set_timeout(struct timespec*);

void prepare_fd_sets(fd_set* read_fds, fd_set* write_fds, int listen_socket_fd);



static int accept_connection(int listen_socket_fd);

static int try_accept_connection(int listen_socket_fd);

static int try_set_nonblock_mode_for_socket(int connection_fd);

static int initialize_new_connection_session(int connection_fd);



static void event_handler(fd_set* read_fds, fd_set* write_fds, int listen_socket_fd);

static void handle_write_event(session_t* session);

static void handle_read_event(session_t* session);

static void change_session_state(session_t* session);

static const dialog_t* get_dialog(dialog_state_t current_dialog_id, int need_next_dialog);



static void prepare_session_for_close(session_t* session);


static void remove_ended_session();

static void close_connection(int connected_fd);








static void start_server(server_context_t* server_context)
{
    server_context = create_server_context();

    load_config();
}

static void stop_server(server_context_t* server_context)
{
    /*destroy data*/



    free_server_context(server_context); /*to this moment all data must be destroy*/
}






/*TODO:this need check*/
static void load_config()
{
    buffer_t* buffer;
    char* path_to_file;

    buffer = create_buffer();

    load_config_file(buffer);

    replace_symbol(buffer->ptr, buffer->size, DIRTY_STRING_SEPARATOR, C_STRING_SEPARATOR);
    
    path_to_file = get_path_data_file(buffer);

    clear_buffer(buffer);

    initialize_config(buffer, path_to_file);

    free(path_to_file);
    free_buffer(buffer);
}

static void load_config_file(buffer_t* buffer)
{
    int config_file_fd;

    config_file_fd = open(CONFIG_PATH, O_RDONLY);

    if (config_file_fd == -1)
    {
        perror("load_config_file");
        exit(EXIT_FAILURE);
    }

    read_from_fd(buffer, config_file_fd);
    close(config_file_fd);
}

static char* get_string(buffer_t* buffer);


static char* get_string(buffer_t* buffer)
{
    char* result;
    int position;

    position = find(buffer, C_STRING_SEPARATOR);

    if (position == -1)
        return NULL;

    position += 1;/*including*/

    result = make_copy_string(buffer->ptr, position);

    move_content_left(buffer, position);

    return result;
}

static char* get_path_data_file(buffer_t* buffer)
{
    char* path_to_file;

    path_to_file = get_string(buffer);
    
    if (!path_to_file)
    {
        perror("get_path_data_file");
        exit(EXIT_FAILURE);
    }

    return path_to_file;
}

static void initialize_config(buffer_t* buffer, const char* path_to_file)
{
    int dialog_file_fd;
    int dialog_id = DEFAULT_DIALOG_ID;
    int result_read;
    char* str;
    dialog_t* dialog;

    dialog_file_fd = open(path_to_file, O_RDONLY);

    if (dialog_file_fd == -1)
    {
        perror("initialize_config open file");
        exit(EXIT_FAILURE);
    }

    while ((result_read = read_from_fd(buffer, dialog_file_fd)))
    {
        replace_symbol(buffer->ptr, buffer->size, DIRTY_STRING_SEPARATOR, C_STRING_SEPARATOR);

        if (result_read == -1)
        {
            perror("initialize_config: read from fd");
            exit(EXIT_FAILURE);/*cant initialize config*/
        }

        while ((str = get_string(buffer)))/*TODO*/
        {
            dialog = create_dialog(dialog_id, str);
            dialog_id++;

            create_node(dialog_list, dialog);

            /*TODO: remove this*/
            printf("%s\n", str);
            /**/
        }
    }

    close(dialog_file_fd);
}







int main()
{
    int listen_socket_fd;
    int max_d;
    int result;
    int result_connection;
    struct timespec timeout;
    fd_set read_fds, write_fds;
    sigset_t mask, oldmask;
    server_context_t* server_context = NULL;

    session_list = create_list();
    dialog_list = create_list();



#ifdef GET_PROCESS_ID_WHEN_START
    printf("%d\n", getpid());
#endif

    setup_signal(&mask, &oldmask);


    
    load_config();
    /*start_server(server_context);*/
    initialize_listen_socket(&listen_socket_fd);

    max_d = listen_socket_fd;
    

    for (;;)
    {
        /*event selection*/
        set_timeout(&timeout);
        prepare_fd_sets(&read_fds, &write_fds, listen_socket_fd);
        
        /*event selection*/

        result = pselect(max_d + 1, &read_fds, &write_fds, NULL, &timeout, &oldmask);

        /*event processing*/

        if (result == -1)/*receive signal*/
        {
            if (server_state == STOP_SERVER)
            {
                /*stop_server(server_context);*/
                break;
            }
            else if (server_state == RELOAD_CONFIG)
            {
                stop_server(server_context);
                start_server(server_context);
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
            if (FD_ISSET(listen_socket_fd, &read_fds))
            {
                result_connection = accept_connection(listen_socket_fd);
                
                if (result_connection != -1)
                    max_d = result_connection;
                result--;
            }

            if (result > 0)
                event_handler(&read_fds, &write_fds, listen_socket_fd);
        }

        remove_ended_session();
        /*event processing*/
    }

    /*TODO: stop without memmory leak*/
    /*-------------------------------*/
    free_list(dialog_list, free_dialog);
    free_list(session_list, free_session);

    close_listen_socket(&listen_socket_fd);
    /*-------------------------------*/

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




















void initialize_listen_socket(int* listen_socket_fd)
{
    int result;
    struct sockaddr_un addr;

    *listen_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (*listen_socket_fd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    result = bind(*listen_socket_fd, (struct sockaddr*)&addr, 
                   sizeof(struct sockaddr_un));

    if (result == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    result = listen(*listen_socket_fd, queue_connection);

    if (result == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}

void close_listen_socket(int* listen_socked_fd)
{
    shutdown(*listen_socked_fd, SHUT_RDWR);
    close(*listen_socked_fd);
    unlink(SOCKET_PATH);
}






void set_timeout(struct timespec* timeout)
{
    timeout->tv_sec = WAITING_TIME_IN_SEC;
    timeout->tv_nsec = WAITING_TIME_IN_NSEC;
}

void prepare_fd_sets(fd_set* read_fds, fd_set* write_fds, int listen_socket_fd)
{
    node_t* node;
    session_t* session;

    FD_ZERO(read_fds);
    FD_ZERO(write_fds);

    FD_SET(listen_socket_fd, read_fds);

    node = session_list->pointer_in_head;

    for (; node; node = node->next)
    {
        if ((session = (session_t*)node->data))
        {
            if (session->state == ready_send_info)
                FD_SET(session->socket_fd, write_fds);
            else if (session->state == ready_receive_info)
                FD_SET(session->socket_fd, read_fds);
        }
    }
}



/*return fd connected client or -1*/
int accept_connection(int listen_socket_fd)
{
    int connected_fd;
    int result;

    connected_fd = try_accept_connection(listen_socket_fd);
    
    if (connected_fd == -1)
        return -1;

    result = try_set_nonblock_mode_for_socket(connected_fd);

    if (result != -1)
    {
        if (initialize_new_connection_session(connected_fd) != -1)
            return connected_fd;    
    }

    return -1;
}
/*TODO:need update*/
static int try_accept_connection(int listen_socket_fd)
{
    int connected_fd;

    connected_fd = accept(listen_socket_fd, NULL, NULL);
    
    if (connected_fd == -1)
        perror("try_accept_connection");

    return connected_fd;
}

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

static int initialize_new_connection_session(int connection_fd)
{
    session_t* session;
    const dialog_t* dialog;
    const char* msg;

    int current_dialog_id = DEFAULT_DIALOG_ID;

    dialog = get_dialog(current_dialog_id, false);

    if (!dialog)
        return -1;
    
    current_dialog_id = dialog->dialog_id;
    msg = dialog->msg;
    
    session = create_session(connection_fd, current_dialog_id, msg);

    create_node(session_list, (void*)session);

    return 0;
}




static void event_handler(fd_set* read_fds, fd_set* write_fds, int listen_socket_fd)
{
    int session_fd;
    node_t* node;
    session_t* session;
    session_state_t state;

    node = session_list->pointer_in_head;

    for (; node; node = node->next)
    {
        if ((session = (session_t*)node->data))
        {
            session_fd = session->socket_fd;
            state = session->state;

            if (state == ready_send_info)
            {
                if (FD_ISSET(session_fd, write_fds))
                    handle_write_event(session);
            }
            else if (state == ready_receive_info)
            {
                if (FD_ISSET(session_fd, read_fds))
                    handle_read_event(session);
            }

            change_session_state(session);//this is why client receive message broken pipe, socket close before client send last answer
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

static void change_session_state(session_t* session)
{
    const char* msg;
    const dialog_t* dialog;
    int current_dialog_id;

    current_dialog_id = session->current_dialog_id;
    dialog = get_dialog(current_dialog_id, true);
    
    if (!dialog)
    {
        prepare_session_for_close(session);
        return;
    }
    
    current_dialog_id = dialog->dialog_id;
    msg = dialog->msg;
    
    try_change_session_state(session, current_dialog_id, msg);
}

/*FIX ME PLEASE!!!*/
/*fix this!!!! if you need take dialog with next id then just current_dialog_id + 1, dont use this need_next_dialog*/
static const dialog_t* get_dialog(dialog_state_t current_dialog_id, int need_next_dialog)
{
    node_t* node;
    dialog_t* dialog;

    node = dialog_list->pointer_in_head;

    for (; node; node = node->next)
    {
        if ((dialog = (dialog_t*)node->data))
        {
            if (dialog->dialog_id == current_dialog_id)
            {
                if (need_next_dialog == true)
                    node = node->next;

                if (!node)
                    return NULL;                
                return (dialog_t*)node->data;
            }
        }
    }
    return NULL;
}
/*FIX ME PLEASE!!!*/


static void prepare_session_for_close(session_t* session)
{
    update_session_current_dialog_id(session, ERROR_DIALOG_ID);
}


/*TODO: need fix undefined behaviour when we remove some session what stored in
get_current, and how we can access to memory?*/
static void remove_ended_session()
{
    int connection_fd;
    int current_dialog_id;
    node_t* node;
    session_t* session;
    node_t** ids_for_close = get_mem(sizeof(node_t) * session_list->count);
    int count_id_close = 0;

    node = session_list->pointer_in_head;

    for (; node; node = node->next)
    {
        if ((session = (session_t*)node->data))
        {
            current_dialog_id = session->current_dialog_id;

            if (current_dialog_id == ERROR_DIALOG_ID)
            {
                connection_fd = session->socket_fd;

                close_connection(connection_fd);

                ids_for_close[count_id_close] = node;
                count_id_close++;
            }
        }
    }
    for (int i = 0; i < count_id_close; i++)
    {
        remove_node(session_list, ids_for_close[i], free_session);
    }

    free(ids_for_close);
}

static void close_connection(int connected_fd)
{
    shutdown(connected_fd, SHUT_RDWR);
    close(connected_fd);
}