#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef MEMORY_H
#define MEMORY_H

#define MEM_SIZE 2048

enum print_mode {
    HEX,
    ASCII
};

struct memory {
    char *bytes;
    size_t s;
};

struct memory *new_memory();
void destroy_memory(struct memory *mem);

int   dump  (struct memory *mem, enum print_mode m);
int   write (struct memory *mem, char *b, size_t s, size_t offset);
char* read  (struct memory *mem, size_t s, size_t offset);

#endif
