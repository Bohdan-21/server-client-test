#include <fcntl.h>
#include <stdlib.h>

#include "buffer.h"

void read_test();

void  write_test();

void get_string_NULL();

void open_config_file();

int main()
{
    read_test();
    write_test();
    get_string_NULL();
    open_config_file();

    return 0;
}

void read_test()
{
    int fd = open("./config/dialog.txt", O_RDONLY);
    buffer_t buffer;
    char* str;

    if (fd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    initialize_buffer(&buffer);

    read_from_fd_to_buffer(&buffer, fd);

    str = get_string(&buffer, '\n');

    printf("%s\n", str);


    str = get_string(&buffer, '\n');

    printf("%s\n", str);


    str = get_string(&buffer, '\n');

    printf("%s\n", str);
    

    str = get_string(&buffer, '\n');

    printf("%s\n", str);

    close(fd);
}

void write_test()
{
    int write_fd = open("./test/write_test", O_CREAT | O_WRONLY, 0644);
    int read_fd = open("./config/dialog.txt", O_RDONLY);

    buffer_t buffer;

    initialize_buffer(&buffer);

    read_from_fd_to_buffer(&buffer, read_fd);
    write_from_buffer_to_fd(write_fd, &buffer);

    close(write_fd);
    close(read_fd);
}

void get_string_NULL()
{
    int fd;
    buffer_t buffer;
    char* str;

    fd = open("./test/without_separator", O_RDONLY);

    initialize_buffer(&buffer);

    read_from_fd_to_buffer(&buffer, fd);

    str = get_string(&buffer, '\n');
    
    if (!str)
        printf("All good separator not find");    
}

void open_config_file()
{
    int confgi_fd;
    int dialog_fd;
    int re_write_fd;
    buffer_t buffer;
    char* dialog_path;

    confgi_fd = open("./config/config.txt", O_RDONLY);

    initialize_buffer(&buffer);

    read_from_fd_to_buffer(&buffer, confgi_fd);

    dialog_path = get_string(&buffer, '\n');

    clear_buffer(&buffer);

    dialog_fd = open(dialog_path, O_RDONLY);
    re_write_fd = open("./test/rewrite.txt", O_CREAT | O_WRONLY, 0644);

    read_from_fd_to_buffer(&buffer, dialog_fd);

    write_from_buffer_to_fd(re_write_fd, &buffer);
}