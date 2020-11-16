#include "ftable.h"

int main(void)
{
    struct ftable *ft = new_ftable();
    unsigned long key = ftable_add_file(ft, "fname", 10, 20);
    printf("key: %lx\n", key);

    return 0;
}
