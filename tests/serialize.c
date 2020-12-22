#include "serialize.h"
#include <stdio.h>
#include <assert.h>

void test_memory()
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
    destroy_memory(mem);
    destroy_memory(d_mem);
}

void test_ftfile()
{
    char *filename = "filename";
    struct ftable_file *ftf = new_ftable_file(filename, 20, 30);
    uint8_t *serialized;
    unsigned len = serialize_ftable_file(&serialized, ftf);
    for (int u = 0; u < len; u++)
        printf("%0x ", serialized[u]);
    printf("\n");
  
   struct ftable_file *deserialized = deserialize_ftable_file(serialized, len);
   printf("deserialized:\n  name: %s\n  size: %ld\noffset: %ld\n",
       deserialized->name, deserialized->s, deserialized->offset
   );

   destroy_ftable_file(ftf);
   destroy_ftable_file(deserialized);
}

int main()
{
    // test_memory();
    test_ftfile();

    return 0;
}
