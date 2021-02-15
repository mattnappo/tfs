#include "util.h"

unsigned long ipow(int b, int x)
{
    unsigned long s = 1;
    for (int i = 0; i < x; i++)
        s *= b;
    return s;
}

unsigned long to_radix(char *str)
{
    unsigned long radix = 0;
    size_t st = strlen(str);
    for (int b = 0; b < st; b++)
        radix += str[b] * ipow(128, st-b-1);
    return radix;
}

void err(char *emsg)
{
    fprintf(stderr, "%s.\n", emsg);
    exit(1);
}
