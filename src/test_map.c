#include "ftable.h"

int main(void)
{
    struct ftable *ft = new_ftable();
    unsigned long key = ftable_add_file(ft, "fname", 10, 20);
    printf("key: %lu\n", key);

    struct ftable_file test_get = ftable_get_file(ft, "fname");
    printf("name: %s\n  s: %zu\n  offset: %zu\n", test_get.name, test_get.s, test_get.offset);

    return 0;
}
