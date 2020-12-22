#include "ftable.h"

void get(struct ftable *ft, char name[])
{
    struct ftable_file test_get = ftable_get_file(ft, name);
    printf("name: %s\n  s: %zu\n  offset: %zu\n\n", test_get.name, test_get.s, test_get.offset);
}

int main(void)
{
    struct ftable *ft = new_ftable();
    unsigned long key = ftable_add_file(ft, "fname", 10, 20);
    printf("key: %lu\n", key);

    get(ft, "name");
    get(ft, "fname");

    unsigned long key1 = ftable_add_file(ft, "abcdef", 33, 44);
    unsigned long key2 = ftable_add_file(ft, "fdebga", 55, 66);
    unsigned long key3 = ftable_add_file(ft, "awesome", 77, 88);
    printf("keys: %zu, %zu, %zu\n", key1, key2, key3);

    get(ft, "abcdef");
    get(ft, "fdebga");
    get(ft, "not");
    get(ft, "awesome");

    printf("testing bucket_get_file_index\n");
    for (int k = 0; k < NUM_BUCKETS; k++) {
        printf("n files: %ld\n", ft->buckets[k]->n_entries);
        for (int j = 0; j < ft->buckets[k]->n_entries; j++) {
            struct ftable_file ftf = bucket_get_file_index(
                ft->buckets[k], j
            );
            printf("name: %s\n  s: %zu\n  offset: %zu\n\n", ftf.name, ftf.s, ftf.offset);
        }
    }

    destroy_ftable(ft);

    return 0;
}
