#include <fcntl.h>
#include <stdlib.h>

#include "../src/modules/buffer.h"
#include "../src/modules/base.h"

void read_test();

void  write_test();

void get_string_NULL();

void open_config_file();

int main()
{
    read_test();
    /*get_string_NULL();
    open_config_file();
*/
    return 0;
}

void read_test()
{
    int fd = open("../config/dialog.txt", O_RDONLY);
    buffer_t* buffer;
    char* str;

    if (fd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    buffer = create_buffer();

    int free_space = get_buffer_free_space(buffer);

    buffer->size += read(fd, buffer->ptr, free_space);

    int position = find(buffer, '\n');

    str = make_copy_string(buffer->ptr, position);

    printf("%s\n", str);

    move_content_left(buffer, position);

    free(str);




    position = find(buffer, '\n');

    str = make_copy_string(buffer->ptr, position);

    printf("%s\n", str);

    move_content_left(buffer, position);

    free(str);



    position = find(buffer, '\n');

    str = make_copy_string(buffer->ptr, position);

    printf("%s\n", str);

    move_content_left(buffer, position);

    free(str);




    close(fd);
}
/*
void write_test()
{
    int write_fd = open("./test/write_test", O_CREAT | O_WRONLY, 0644);
    int read_fd = open("./config/dialog.txt", O_RDONLY);

    buffer_t* buffer;

    buffer = create_buffer();

    read_to_buffer_from_fd(buffer, read_fd);
    write_to_fd_from_buffer(write_fd, buffer);

    free_buffer(buffer);

    close(write_fd);
    close(read_fd);
}

void get_string_NULL()
{
    int fd;
    buffer_t* buffer;
    char* str;

    fd = open("./test/without_separator", O_RDONLY);

    buffer = create_buffer();

    read_to_buffer_from_fd(buffer, fd);

    str = get_string(buffer, '\n');
    
    if (!str)
        printf("All good separator not find");    

    free_buffer(buffer);
}

void open_config_file()
{
    int confgi_fd;
    int dialog_fd;
    int re_write_fd;
    buffer_t* buffer;
    char* dialog_path;

    confgi_fd = open("./config/config.txt", O_RDONLY);

    buffer = create_buffer();

    read_to_buffer_from_fd(buffer, confgi_fd);

    close(confgi_fd);


    dialog_path = get_string(buffer, '\n');

    clear_buffer(buffer);


    dialog_fd = open(dialog_path, O_RDONLY);

    free(dialog_path);

    re_write_fd = open("./test/rewrite.txt", O_CREAT | O_WRONLY, 0644);

    read_to_buffer_from_fd(buffer, dialog_fd);

    close(dialog_fd);


    write_to_fd_from_buffer(re_write_fd, buffer);

    free_buffer(buffer);

    close(re_write_fd);
}*/