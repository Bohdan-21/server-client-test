#include <stdio.h>

#include "../src/modules/buffer.h"

int main()
{
    buffer_t* buffer = create_buffer();
    copy_string_to_buffer(buffer, "\n");
    int pos;
    if ((pos = find_string(buffer)))
    {
        printf("find separator in position %d\n", pos);
    }

    return 0;
}