#include <stdio.h>
#include <fcntl.h>

#include "../src/modules/buffer.h"
#include "../src/modules/custom_io.h"


int main()
{
    int fd = open("test/file_data_for_test_custom_io.txt", O_RDONLY);


    if (fd == -1)
        return 1;

    buffer_t* buffer;

    buffer = create_buffer();

    read_from_fd(buffer, fd);

    char* msg;
    while((msg = get_string(buffer, '\n')))
    {
        printf("%s\n", msg);
        free(msg);
        read_from_fd(buffer, fd);
    }


    return 0;
}


