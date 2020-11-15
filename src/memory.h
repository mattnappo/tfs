#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef MEMORY_H
#define MEMORY_H

#define MEM_SIZE 1024

enum print_mode {
  HEX,
  ASCII
};

struct memory {
  char *bytes;
  size_t s;
};

// These are all of the basic operations. Nothing else needs to be
// implemented for a basic memory.

// Construct a new memory struct
struct memory *new_memory();

// Print out the contents of a memory
int dump(struct memory *mem, enum print_mode m);

// Write bytes to a memory
int write(struct memory *mem, char *b, size_t s, size_t offset);

// Read some bytes from a memory.
char *read(struct memory *mem, size_t s, size_t offset);

/* TODO
// Memory functions
struct pointer {
    something
}
size_t memmalloc(size_t size);
size_t memcalloc(size_t n, size_t size);
void free(void *);
*/

#endif
