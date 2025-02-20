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

#define NOTHING_DO 0
#define RELOAD_CONFIG 1
#define STOP_SERVER 2

#define WAITING_TIME_IN_SEC 5
#define WAITING_TIME_IN_NSEC 0

#define STRING_SEPARATOR '\n'
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



void signal_handler(int);


void initialize_listen_socket(int*);

void close_listen_socket(int*);


void setup_signal(sigset_t* mask, sigset_t* oldmask);

void setup_signal_mask(sigset_t* mask, sigset_t* oldmask);



static void load_config();

static void load_config_file(buffer_t* config_data);

static char* get_path_data_file(buffer_t* config_data);

static void initialize_config(buffer_t* config_data, const char* path_to_file);



void set_timeout(struct timespec*);

void prepare_fd_sets(fd_set* read_fds, fd_set* write_fds, int listen_socket_fd);



static int accept_connection(int listen_socket_fd);

static int try_accept_connection(int listen_socket_fd);

static int try_set_nonblock_mode_for_socket(int connection_fd);

static void initialize_new_connection_session(int connection_fd);



static void event_handler(fd_set* read_fds, fd_set* write_fds, int listen_socket_fd);

static void handle_write_event(session_t* session);

static void handle_read_event(session_t* session);

static void change_session_state(session_t* session);

static const dialog_t* get_dialog(dialog_state_t current_dialog_id, int need_next_dialog);



static void prepare_session_for_close(session_t* session);


static void remove_ended_session();

static void close_connection(int connected_fd);





int main()
{
    int listen_socket_fd;
    int max_d;
    int result;
    int result_connection;
    struct timespec timeout;
    fd_set read_fds, write_fds;
    sigset_t mask, oldmask;

#ifdef GET_PROCESS_ID_WHEN_START
    printf("%d\n", getpid());
#endif

    setup_signal(&mask, &oldmask);
    initialize_lists();
    load_config();
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

        if (result == -1)
        {
            printf("Receive singal\n");
            
            if (server_state == STOP_SERVER)
                break;
            else if (server_state == RELOAD_CONFIG)
                server_state = NOTHING_DO;
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
            }

            if (result > 1)
                event_handler(&read_fds, &write_fds, listen_socket_fd);
        }

        remove_ended_session();
        /*event processing*/
    }

    /*TODO: stop without memmory leak*/
    /*-------------------------------*/
    free_lists();
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



static void load_config()
{
    buffer_t* buffer;
    char* path_to_file;

    buffer = create_buffer();

    load_config_file(buffer);
    
    path_to_file = get_path_data_file(buffer);

    clear_buffer(buffer);

    initialize_config(buffer, path_to_file);

    free(path_to_file);
    free_buffer(buffer);
}

static void load_config_file(buffer_t* config_data)
{
    int config_file_fd;

    config_file_fd = open(CONFIG_PATH, O_RDONLY);

    if (config_file_fd == -1)
    {
        perror("load_config_file");
        exit(EXIT_FAILURE);
    }

    read_from_fd(config_data, config_file_fd);
    close(config_file_fd);
}

static char* get_path_data_file(buffer_t* config_data)
{
    char* path_to_file;

    path_to_file = get_string(config_data, STRING_SEPARATOR);
    path_to_file = make_c_string(path_to_file);

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
        if (result_read == -1)
        {
            exit(EXIT_FAILURE);/*cant initialize config*/
        }

        while ((str = get_string(buffer, STRING_SEPARATOR)))
        {
            dialog = create_dialog_t(dialog_id, str);
            dialog_id++;

            create_node(dialog_type, dialog);

            /*TODO: remove this*/
            str = make_c_string(str, STRING_SEPARATOR);
            printf("%s\n", str);
            /**/

            free(str);
        }
    }

    close(dialog_file_fd);
}



void set_timeout(struct timespec* timeout)
{
    timeout->tv_sec = WAITING_TIME_IN_SEC;
    timeout->tv_nsec = WAITING_TIME_IN_NSEC;
}

void prepare_fd_sets(fd_set* read_fds, fd_set* write_fds, int listen_socket_fd)
{
    session_t* session;

    FD_ZERO(read_fds);
    FD_ZERO(write_fds);

    FD_SET(listen_socket_fd, read_fds);

    reset_current(session_type);

    while((session = (session_t*)(get_current(session_type))))
    {
        if (session->state == ready_send_info)
            FD_SET(session->socket_fd, write_fds);
        else if (session->state == ready_receive_info)
            FD_SET(session->socket_fd, read_fds);

        move_next(session_type);
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
        initialize_new_connection_session(connected_fd);
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

static void initialize_new_connection_session(int connection_fd)
{
    session_t* session;
    const dialog_t* dialog;
    const char* msg;

    int current_dialog_id = DEFAULT_DIALOG_ID;

    dialog = get_dialog(current_dialog_id, false);

    if (!dialog)
        return;
    
    current_dialog_id = dialog->dialog_id;
    msg = dialog->msg;
    
    session = create_session(connection_fd, current_dialog_id, msg);

    create_node(session_type, (void*)session);
}




static void event_handler(fd_set* read_fds, fd_set* write_fds, int listen_socket_fd)
{
    int session_fd;
    session_t* session;
    session_state_t state;

    reset_current(session_type);

    while((session = (session_t*)get_current(session_type)))
    {
        session_fd = get_session_fd(session);
        state = get_session_state(session);

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

        change_session_state(session);
        move_next(session_type);
    }
}

static void handle_write_event(session_t* session)
{
    int result_operation;
    int session_fd;
    buffer_t* buffer;

    session_fd = get_session_fd(session);
    buffer = get_session_buffer(session);

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

    session_fd = get_session_fd(session);
    buffer = get_session_buffer(session);

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

    current_dialog_id = get_session_current_dialog_id(session);
    dialog = get_dialog(current_dialog_id, true);
    
    if (!dialog)
    {
        prepare_session_for_close(session);
        return;
    }
    
    current_dialog_id = dialog->dialog_id;
    msg = dialog->msg;
    
    try_change_session_state(session, current_dialog_id, msg, STRING_SEPARATOR);
}

static const dialog_t* get_dialog(dialog_state_t current_dialog_id, int need_next_dialog)
{
    dialog_t* dialog;

    reset_current(dialog_type);

    while ((dialog = (dialog_t*)get_current(dialog_type)))
    {
        if (dialog->dialog_id == current_dialog_id)
        {
            if (need_next_dialog == true)
                move_next(dialog_type);
            return (dialog_t*)get_current(dialog_type);
        }
        move_next(dialog_type);
    }

    return NULL;
}



static void prepare_session_for_close(session_t* session)
{
    update_session_current_dialog_id(session, ERROR_DIALOG_ID);
}



static void remove_ended_session()
{
    int connection_fd;
    int current_dialog_id;
    session_t* session;

    reset_current(session_type);

    while((session = (session_t*)get_current(session_type)))
    {
        current_dialog_id = get_session_current_dialog_id(session);

        if (current_dialog_id == ERROR_DIALOG_ID)
        {
            connection_fd = get_session_fd(session);

            close_connection(connection_fd);
            remove_session(connection_fd);
        }

        move_next(session_type);
    }
}

static void close_connection(int connected_fd)
{
    shutdown(connected_fd, SHUT_RDWR);
    close(connected_fd);
}













/*#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

#include "session.h"
#include "dialog.h"


#define SOCKET_PATH "./socket_file"


enum 
{
    queue_connection = 5,
};


void handle_connected_client(int);


int main()
{
    int listen_socket;
    struct sockaddr_un addr;

    listen_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    if (listen_socket == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }


    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);


    if (bind(listen_socket, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(listen_socket, queue_connection) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }


    handle_connected_client(listen_socket);

    exit(EXIT_SUCCESS);
}


void handle_connected_client(int listen_socket)
{
    fd_set readfds1, writefds1;
    struct timeval timeout;
    int result;
    int connected_client = 0;

    timeout.tv_sec = 60;
    timeout.tv_usec = 0;

    FD_SET(listen_socket, &readfds1);

    result = select(listen_socket + 1, &readfds1, &writefds1, NULL, &timeout);

    if (result == -1)
    {
        perror("first select");
        return;
    }
    else if (result == 0)
    {
        perror("first select time out");
        return;
    }
    else if (result > 0)
    {
        connected_client = accept(listen_socket, NULL, NULL);
    }

    write(connected_client, "Hello\n", 7);

    shutdown(connected_client, SHUT_RDWR);

    close(connected_client);
    close(listen_socket);

    unlink(SOCKET_PATH);
}*/




/*#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <string.h>
#include "list.h"
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>

#define SOCKET_PATH "./socket_file"

enum
{
    listen_queue = 5
};


enum sd_status
{
    have_info_for_read,
    have_info_for_write,
};


void setup_fd_sets(fd_set*, fd_set*, int*);

void handle_events(fd_set*, fd_set*, int);

void handler_new_connection(int);

void handle_socket_descriptor_state(struct session**, fd_set*, fd_set*);

void select_handler(struct session**, enum sd_status);

void handler_read_event(struct session**);

void handler_write_event(struct session**);

void end_connection(struct session**);

int main()
{
    int sd;
    int bind_result;
    int listen_result;
    int result_select;
    struct sockaddr_un addr;

    sd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (sd == -1)
    {
        perror("Socket");
        exit(EXIT_FAILURE);
    }

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    bind_result = bind(sd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un));

    if (bind_result == -1)
    {
        perror("Bind");
        exit(EXIT_FAILURE);
    }

    listen_result = listen(sd, listen_queue);

    if (listen_result == -1)
    {
        perror("Listen");
        exit(EXIT_FAILURE);
    }

    create_node(sd, listen_connection);

    for(;;)
    {
        fd_set readfds, writefds;
        int max_d = sd;
        struct timeval timeout;

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;


        setup_fd_sets(&readfds, &writefds, &max_d);

        result_select = select(max_d + 1, &readfds, &writefds, NULL, &timeout);

        if (result_select == -1)
            perror("Select");
        else if (result_select == 0)
            continue;
        else if (result_select > 0)
        {
            handle_events(&readfds, &writefds, result_select);
        }
    }

    exit(EXIT_SUCCESS);
}












void setup_fd_sets(fd_set* readfds, fd_set* writefds, int* max_d)
{
    struct session** tmp;

    FD_ZERO(readfds);
    FD_ZERO(writefds);

    for (reset_current();(tmp = get_current()); move_next())
    {
        FD_SET((*tmp)->sd, readfds);
        if have info for write or doest receive info for write
        if (need_write(*tmp) > 0 || !received_info_for_write(*tmp))
        {
            FD_SET((*tmp)->sd, writefds);
        }
        if ((*tmp)->sd > *max_d)
            *max_d = (*tmp)->sd;
    }
}


void handle_events(fd_set* readfds, fd_set* writefds, int number_of_events)
{
    struct session** tmp;

    for (reset_current();(tmp = get_current());)
    {
        if (FD_ISSET((*tmp).sd, readfds) != 0)
        {
            if ((*tmp).status == listen_connection)
                setup_new_connection((*tmp).sd);
            else
                handler_read_event(tmp);

            number_of_events--;
        }
        else if (FD_ISSET((*tmp).sd, writefds) != 0)
        {
            handler_write_event(tmp);
            number_of_events--;
        }
        if (FD_ISSET((**tmp).sd, readfds) != 0)
        {
            if ((**tmp).status == listen_connection)
                handler_new_connection((**tmp).sd);
            move_next();
        }
        else
            handle_socket_descriptor_state(tmp, readfds, writefds);
        
        number_of_events--;
    }

    if (number_of_events != 0)
        perror("Handle events");

    return;
}


void handler_new_connection(int sd)
{
    int connected_sd;
    int flags;

    connected_sd = accept(sd, NULL, NULL);

    if (connected_sd == -1)
    {
        perror("Accept");
        return;
    }

    flags = fcntl(connected_sd, F_GETFL);

    if (flags == -1)
        perror("F_GETFL");
    if (fcntl(connected_sd, F_SETFL, flags | O_NONBLOCK) == -1)
        perror("F_SETFL");

    create_node(connected_sd, welcome_state);

    return;
}

void handle_socket_descriptor_state(struct session** client, fd_set* readfds, fd_set* writefds)
{
    enum sd_status sd_st;

    if (FD_ISSET((**client).sd, readfds))
        sd_st = have_info_for_read;
    else if (FD_ISSET((**client).sd, writefds))
        sd_st = have_info_for_write;

    select_handler(client, sd_st);
}

void select_handler(struct session** client, enum sd_status sd_st)
{
    if (sd_st == have_info_for_write)
        handler_write_event(client);
    else
        handler_read_event(client);
}

void handler_read_event(struct session** client)
{    
    int result;
    switch ((**client).status)
    {
        case welcome_state:
            result = read((**client).sd, (**client).buff_read, 128);
            if (result == 0)
            {
                end_connection(client);
            }
            else
            {
                printf("%s", (**client).buff_read);
                (**client).status = age_state;
                move_next();
            }
        break;
        case age_state:
            result = read((**client).sd, (**client).buff_read, 128);
            if (result == 0)
            {
                end_connection(client);
            }
            else
            {
                printf("%s", (**client).buff_read);
                (**client).status = town_state;
                move_next();
            }
        break;
        case town_state:
            result = read((**client).sd, (**client).buff_read, 128);
            if (result == 0)
            {
                end_connection(client);
            }
            else
            {
                printf("%s", (**client).buff_read);
                (**client).status = exit_state;
                move_next();
            }
        break;
        case exit_state:
            result = read((**client).sd, (**client).buff_read, 128);
            if (result == 0)
            {
                end_connection(client);
            }
            else
            {
                printf("%s", (**client).buff_read);
                (**client).status = age_state;
                end_connection(client);
            }
        break;
        default:
        break;
    }
}

void handler_write_event(struct session** client)
{
    switch ((**client).status)
    {
        case welcome_state:
            if (!received_info_for_write(*client))
            {
                set_write_info(*client, "Hello client!");
            }
            else
            {
                write((**client).sd, (**client).buff_write, (**client).size_write_buff);
                move_next();
            }
        break;
        case age_state:
            if (!received_info_for_write(*client))
            {
                set_write_info(*client, "Hello client second time!");
            }
            else
            {
                write((**client).sd, (**client).buff_write, (**client).size_write_buff);
                move_next();
            }
        break;
        case town_state:
            if (!received_info_for_write(*client))
            {
                set_write_info(*client, "Hello client third time!");
            }
            else
            {
                write((**client).sd, (**client).buff_write, (**client).size_write_buff);
                move_next();
            }
        break;
        case exit_state:
            if (!received_info_for_write(*client))
            {
                set_write_info(*client, "Bye client!");
            }
            else
            {
                write((**client).sd, (**client).buff_write, (**client).size_write_buff);
                end_connection(client);
            }
        break;
        default:
        break;
    }
}

void end_connection(struct session** session)
{
    struct session* rm_node = (*session);
    move_next();
    remove_node((*rm_node).sd);
}*/