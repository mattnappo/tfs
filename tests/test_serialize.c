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
    uint8_t *s = "test bytes";
    struct memory *mem = new_memory();
    write(mem, s, 10, 0);

    // Serialize
    uint8_t *serialized_buf;
    // Returns the length of the serialized bytes
    unsigned len = serialize_memory(&serialized_buf, mem);

    for (int u = 0; u < len; u++)
        printf("%0x ", serialized_buf[u]);

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
    ftable_add_file(ft, "fname", 10, 20);
    ftable_add_file(ft, "abcdef",  33, 44);
    ftable_add_file(ft, "fdebga",  55, 66);
    ftable_add_file(ft, "awesome", 77, 88);

    for (int k = 0; k < NUM_BUCKETS; k++) {
        if (ft->buckets[k]->n_entries > 0) {
            // Serialize
            uint8_t *buffer;
            unsigned blen = serialize_ftable_bucket(&buffer, ft->buckets[k]);
            printf("bucket: \n  ");
            for (int u = 0; u < blen; u++)
                printf("%x ", buffer[u]);
            printf("\n");

            // Deserialize
            struct ftable_bucket *deserialized =
                deserialize_ftable_bucket(buffer, blen);

            struct ftable_file *temp = deserialized->head;
            while (temp != NULL) {
                print_ftable_file(*temp);
                temp = temp->next;
            }

            destroy_ftable_file(temp);
            destroy_ftable_bucket(deserialized);
            free(buffer);
        }
    }

    destroy_ftable(ft);
}

void test_ftable()
{
    // unsigned serialize_ftable_bucket(uint8_t **buf, struct ftable_bucket *bucket)
    struct ftable *ft = new_ftable();
    ftable_add_file(ft, "fname", 10, 20);
    ftable_add_file(ft, "abcdef",  33, 44);
    ftable_add_file(ft, "fdebga",  55, 66);
    ftable_add_file(ft, "awesome", 77, 88);

    // serialize
    uint8_t *buffer;
    unsigned slen = serialize_ftable(&buffer, ft);

    // deserialize
    struct ftable *dft = deserialize_ftable(buffer, slen);

    // print deserialized
    printf("deserialized: \n");
    for (int k = 0; k < NUM_BUCKETS; k++) {
        if (dft->buckets[k]->n_entries > 0) {
            struct ftable_file *temp = dft->buckets[k]->head;
            while (temp != NULL) {
                print_ftable_file(*temp);
                temp = temp->next;
            }
            destroy_ftable_file(temp);
        }
    }

    destroy_ftable(dft);

    destroy_ftable(ft);
    free(buffer);
}

void test_fs()
{
    struct fs *tfs = new_fs();
    struct file tfile1 = new_file("testfile.txt");
    struct file tfile2 = new_file("testfile2.txt");
    struct file tfile3 = new_file("Makefile");

    add_file(tfs, tfile2, 0);
    add_file(tfs, tfile1, 100);
    add_file(tfs, tfile3, 200);
    add_file(tfs, tfile3, 600);

    dump(tfs->mem, HEX);
    print_ftable(*tfs->ft);

    get(tfs, "filename");
    get(tfs, "testfile.txt");
    get(tfs, "testfile2.txt");
    get(tfs, "Makefile");

    // Serialize
    uint8_t *buffer;
    unsigned slen = serialize_fs(&buffer, tfs);

    // Deserialize
    struct fs *deserialized = deserialize_fs(buffer, slen);
    dump(deserialized->mem, HEX);
    print_ftable(*deserialized->ft);

    get(deserialized->ft, "filename");
    get(deserialized->ft, "testfile.txt");
    get(deserialized->ft, "testfile2.txt");
    get(deserialized->ft, "Makefile");
    destroy_fs(deserialized);

    free(buffer);

    destroy_file(tfile1);
    destroy_file(tfile2);
    destroy_file(tfile3);
    destroy_fs(tfs);
}

int main()
{
    test_memory();
    test_ftfile();
    test_ftbucket();
    test_ftable();
    test_fs();

    return 0;
}
