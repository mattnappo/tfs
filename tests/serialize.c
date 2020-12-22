#include "serialize.h"
#include <stdio.h>
#include <assert.h>

void get(struct ftable *ft, char name[])
{
    struct ftable_file test_get = ftable_get_file(ft, name);
    printf("name: %s\n  s: %zu\n  offset: %zu\n\n", test_get.name, test_get.s, test_get.offset);
}

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

void test_ftbucket()
{
    // unsigned serialize_ftable_bucket(uint8_t **buf, struct ftable_bucket *bucket)
    struct ftable *ft = new_ftable();
    unsigned long key = ftable_add_file(ft, "fname", 10, 20);
    printf("key: %lu\n", key);

    get(ft, "name");
    get(ft, "fname");

    unsigned long key1 = ftable_add_file(ft, "abcdef",  33, 44);
    unsigned long key2 = ftable_add_file(ft, "fdebga",  55, 66);
    unsigned long key3 = ftable_add_file(ft, "awesome", 77, 88);
    printf("keys: %zu, %zu, %zu\n", key1, key2, key3);

    get(ft, "abcdef");
    get(ft, "fdebga");
    get(ft, "not");
    get(ft, "awesome");

    printf("testing bucket_get_file_index\n");
    for (int k = 0; k < NUM_BUCKETS; k++) {
        if (ft->buckets[k]->n_entries > 0) {
            uint8_t *buffer;
            unsigned blen = serialize_ftable_bucket(&buffer, ft->buckets[k]);
            printf("bucket: \n  ");
            for (int u = 0; u < blen; u++)
                printf("%x ", buffer[u]);
            printf("\n");

            /* deserialize */

            free(buffer);
        }
    }

    destroy_ftable(ft);
}

int main()
{
    // test_memory();
    // test_ftfile();
    test_ftbucket();

    return 0;
}
