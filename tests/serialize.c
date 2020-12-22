#include "serialize.h"
#include <stdio.h>

int main()
{
    char *s = "test bytes";
    struct memory *mem = new_memory();
    write(mem, s, strlen(s), 0);

    uint8_t *buf;
    unsigned len = serialize_memory(&buf, mem);

    for (int u = 0; u < len; u++)
        printf("%0x ", buf[u]);

    destroy_memory(mem);
    return 0;
}
