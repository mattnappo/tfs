#include "serialize.h"
#include <stdio.h>

int main()
{
    // Make a memory
    char *s = "test bytes";
    struct memory *mem = new_memory();
    write(mem, s, strlen(s), 0);

    // Serialize
    uint8_t *serialized_buf;
    // Returns the length of the serialized bytes
    unsigned len = serialize_memory(&serialized_buf, mem);

    for (int u = 0; u < len; u++)
        printf("%0x ", serialized_buf[u]);

    destroy_memory(mem);

    // Deserialize into a new mem*
    struct memory *d_mem = deserialize_memory(serialized_buf, len);

    dump(d_mem, HEX);

    return 0;
}
