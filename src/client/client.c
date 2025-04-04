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
#include "../modules/client_connection.h"

#define SOCKET_PATH "./socket_file"


#define NOTHING_DO 0
#define STOP_CLIENT 1

#define WAITING_TIME_IN_SEC 5
#define WAITING_TIME_IN_NSEC 0


#define STANDART_INPUT 0    /*stdin*/
#define STANDART_OUTPUT 1   /*stdout*/




volatile sig_atomic_t client_programm_state = NOTHING_DO;



void signal_handler(int);



static client_connection_t* initialize_connection(int* max_d);

static int make_connection();

static void set_nonblock_mode_fd(int fd);



void setup_signal(sigset_t* mask, sigset_t* oldmask);

void setup_signal_mask(sigset_t* mask, sigset_t* oldmask);



void set_timeout(struct timespec*);

void prepare_fd_sets(fd_set* read_fds, fd_set* write_fds, client_connection_t* client);


static int handle_event(client_connection_t* client, fd_set *read_fds, fd_set *write_fds);

static int handle_read(client_connection_t* client, fd_set* read_fds);

static int handle_write(client_connection_t* client, fd_set* write_fds);



static int handle_client(client_connection_t* client, fd_set *read_fds, fd_set *write_fds);

/*void reset_terminal_mode(struct termios* term) {
    tcsetattr(STDIN_FILENO, TCSANOW, term);
}

void set_raw_mode(struct termios* term) {
    struct termios raw;
    tcgetattr(STDIN_FILENO, term); // Сохраняем оригинальные настройки
    atexit(reset_terminal_mode); // Гарантируем восстановление при выходе

    raw = *term;
    //raw.c_lflag &= ~(ECHO | ICANON | ISIG); // Выключаем эхо, канонический режим и сигналы
    raw.c_cc[VMIN] = 1;  // Читаем по 1 символу
    raw.c_cc[VTIME] = 0; // Без задержек

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}*/


int main()
{
    int max_d;
    int result_pselect;
    int result_handle_event;
    struct timespec timeout;
    fd_set read_fds, write_fds;
    sigset_t mask, oldmask;
    client_connection_t* client_connection;
    
    /*struct termios term;*/

    /*set_raw_mode(&term);*/

    client_connection = initialize_connection(&max_d);

    setup_signal(&mask, &oldmask);

    for (;;)
    {
        /*event selection*/
        set_timeout(&timeout);

        prepare_fd_sets(&read_fds, &write_fds, client_connection);
        /*event selection*/
        
        result_pselect = pselect(max_d + 1, &read_fds, &write_fds, NULL, &timeout, &oldmask);

        /*event processing*/
        if (result_pselect == -1)
        {
            if (client_programm_state == STOP_CLIENT)
                break;
        }
        else if (result_pselect > 0)
        {
            result_handle_event = handle_event(client_connection, &read_fds, &write_fds);

            if (result_handle_event == -1)
                break;
        }
        /*event processing*/
    }

    close_client_connection(client_connection);
    free_client_connection(client_connection);


    /*reset_terminal_mode(&term);*/

    exit(EXIT_SUCCESS);
/*

    char msg[10];
    int size_msg;
    int sd;
    struct sockaddr_un addr;
    struct timeval timeout;
    fd_set readfds;
    int result;

    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    sd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (sd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (connect(sd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    FD_SET(sd, &readfds);

    result = select(sd + 1, &readfds, NULL, NULL, &timeout);

    if (result == -1)
    {
        perror("select");
        exit(EXIT_FAILURE);
    }
    else if (result == 0)
    {
        perror("select time is out");
        exit(EXIT_FAILURE);
    }
    else if (result > 0)
    {
        FD_ZERO(&readfds);

        size_msg = read(sd, msg, 10);

        printf("%s", msg);
    }

    FD_SET(sd, &readfds);

    result = select(sd + 1, &readfds, NULL, NULL, &timeout);


    if (result == -1)
    {
        perror("select2");
        exit(EXIT_FAILURE);
    }
    else if (result == 0)
    {
        perror("select2 time is out");
        exit(EXIT_FAILURE);
    }
    else if (result > 0)
    {
        FD_ZERO(&readfds);

        size_msg = read(sd, msg, 10);

        if (size_msg == 0)
        {
            printf("%s", "Connection close!\n");
        }
    }

    shutdown(sd, SHUT_RDWR);
    close(sd);

    exit(EXIT_SUCCESS);*/
}



void signal_handler(int signum)
{
    signal(signum, signal_handler);

    if (client_programm_state == NOTHING_DO)
    {
        if (signum == SIGINT || signum == SIGPIPE)/*server call shutdown*/
            client_programm_state = STOP_CLIENT;        
    }
}



static client_connection_t* initialize_connection(int* max_d)
{
    int socket_fd;
    client_connection_t* client_connection;
    
    *max_d = socket_fd = make_connection();/*strange*/

    set_nonblock_mode_fd(STANDART_INPUT);
    set_nonblock_mode_fd(STANDART_OUTPUT);
    set_nonblock_mode_fd(socket_fd);

    client_connection = create_client_connection(STANDART_INPUT, 
                                                 STANDART_OUTPUT,
                                                 socket_fd);

    return client_connection;
}

static int make_connection()
{
    int socket_fd;
    int result_connection;
    struct sockaddr_un addr;

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (socket_fd == -1)
    {
        perror("initialize connection: socket");
        exit(EXIT_FAILURE);
    }

    result_connection = connect(socket_fd, (struct sockaddr*)&addr, 
                                sizeof(struct sockaddr_un));

    if (result_connection == -1)
    {
        perror("initialize connection: connect");
        exit(EXIT_FAILURE);
    }

    return socket_fd;
}

static void set_nonblock_mode_fd(int fd)
{
    int flags;

    flags = fcntl(fd, F_GETFL);

    if (flags == -1)
    {
        perror("set_nonblock mode fd: get");
        exit(EXIT_FAILURE);
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        perror("set_nonblock mode fd: set");
        exit(EXIT_FAILURE);
    }
}





void setup_signal(sigset_t* mask, sigset_t* oldmask)
{
    signal(SIGINT, signal_handler);
    signal(SIGPIPE, signal_handler);
    setup_signal_mask(mask, oldmask);
}

void setup_signal_mask(sigset_t* mask, sigset_t* oldmask)
{
    sigemptyset(mask);
    sigaddset(mask, SIGINT);
    sigaddset(mask, SIGPIPE);
    sigprocmask(SIG_BLOCK, mask, oldmask);
}



void set_timeout(struct timespec* timeout)
{
    timeout->tv_sec = WAITING_TIME_IN_SEC;
    timeout->tv_nsec = WAITING_TIME_IN_NSEC;
}

void prepare_fd_sets(fd_set* read_fds, fd_set* write_fds, client_connection_t* client)
{
    FD_ZERO(read_fds);
    FD_ZERO(write_fds);

    /*least resistance*/
    if (client->client_state == ready_receive_info_from_server)
        FD_SET(client->socket_fd, read_fds);
    else if (client->client_state == ready_receive_info_from_client)
        FD_SET(client->input_fd, read_fds);
    else if (client->client_state == ready_showing_info_for_client)
        FD_SET(client->output_fd, write_fds);
    else if (client->client_state == ready_send_info_to_server)
        FD_SET(client->socket_fd, write_fds);
    /*least resistance*/
}


/*return 0 or -1*/
static int handle_event(client_connection_t* client, fd_set *read_fds, fd_set *write_fds)
{
    int result;

    if (client->client_state == ready_receive_info_from_server ||
        client->client_state == ready_receive_info_from_client)/*read*/

        result = handle_read(client, read_fds);


    else /*if (client->client_state == ready_showing_info_for_client ||
             client->client_state == ready_send_info_to_server)*//*write*/
    
        result = handle_write(client, write_fds);

    change_client_state(client);


    if (result == -1 || result == 0)
        return -1;

    return 0;
}

static int handle_read(client_connection_t* client, fd_set* read_fds)
{
    int result = -1;

    if (FD_ISSET(client->input_fd, read_fds))
    {
        result = read_from_fd(client->input_buffer, client->input_fd);
        printf("rc i\n");
    }
    else if (FD_ISSET(client->socket_fd, read_fds))
    {
        result = read_from_fd(client->server_buffer, client->socket_fd);
    }

    return result;
}

static int handle_write(client_connection_t* client, fd_set* write_fds)
{
    int result = -1;

    if (FD_ISSET(client->output_fd, write_fds))
    {
        result = write_to_fd(client->output_fd, client->server_buffer);
    }
    else if (FD_ISSET(client->socket_fd, write_fds))
    {
        result = write_to_fd(client->socket_fd, client->input_buffer);
    }
    
    return result;
}


/*return -1 */
static int handle_client(client_connection_t* client, fd_set *read_fds, fd_set *write_fds)
{
    int result = -1;

    /*what in this case main, client state or result pselect?*/
    switch(client->client_state)
    {
        case ready_receive_info_from_server:
            if (FD_ISSET(client->socket_fd, read_fds))
            {
                result = read_from_fd(client->server_buffer, client->socket_fd);
            }
        break;
        case ready_receive_info_from_client:
            if (FD_ISSET(client->input_fd, read_fds))
            {
                result = read_from_fd(client->input_buffer, client->input_fd);
            }
        break;
        case ready_showing_info_for_client:
            if (FD_ISSET(client->output_fd, write_fds))
            {
                result = write_to_fd(client->output_fd, client->server_buffer);
            }
        break;
        case ready_send_info_to_server:
            if (FD_ISSET(client->socket_fd, write_fds))
            {
                result = write_to_fd(client->socket_fd, client->input_buffer);
            }
        break;
    }

    return result;
}