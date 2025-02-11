#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>

#define SOCKET_PATH "./socket_file"

int main()
{
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

    exit(EXIT_SUCCESS);
}