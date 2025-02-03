/*
SIGUSR1 - reload config
SIGINT - stop server
*/

#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <string.h>
#include <time.h>
#include <sys/select.h>
#include <fcntl.h>

#include "list.h"
#include "buffer.h"

#define NOTHING_DO 0
#define RELOAD_CONFIG 1
#define STOP_SERVER 2

#define SOCKET_PATH "./socket_file"
#define CONFIG_PATH "./config/config.txt"




enum 
{
    queue_connection = 5,
};



volatile sig_atomic_t server_state = NOTHING_DO;



void signal_handler(int);


void initialize_listen_socket(int*);

void set_timeout(struct timespec*);

void prepare_fd_sets(fd_set*, fd_set*, int);



void load_config();


int main()
{
    int listen_socket_fd;
    int max_d;
    int result;
    struct timespec timeout;
    fd_set read_fds, write_fds;

    initialize_lists();
    load_config();
    initialize_listen_socket(&listen_socket_fd);

    max_d = listen_socket_fd;

    for (;;)
    {
        /*event selection*/
        set_timeout(&timeout);

        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);

        FD_SET(listen_socket_fd, &read_fds);

        /*event selection*/
        /*event processing*/



        /*event processing*/
    }

    exit(EXIT_SUCCESS);
}



void signal_handler(int signum)
{

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

void set_timeout(struct timespec* timeout)
{
    timeout->tv_sec = 5;
    timeout->tv_nsec = 0;
}



void load_config()
{
    int config_file_fd;
    int dialog_file_fd;
    buffer_t buffer;
    char* readed_line;

    initialize_buffer(&buffer);

    config_file_fd = open(CONFIG_PATH, O_RDONLY);

    if (config_file_fd == -1)
    {
        perror("open config file");
        exit(EXIT_FAILURE);
    }

    read_from_fd_to_buffer(config_file_fd, &buffer);



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