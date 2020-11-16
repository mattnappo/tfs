#ifndef FTABLE_H
#define FTABLE_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "util.h"

// A power of 2
#define NUM_BUCKETS 16
#define FILENAME_SIZE 16

// An entry in an ftable bucket
struct ftable_file {
    char name[FILENAME_SIZE];
    size_t s;
    size_t offset;

    struct ftable_file *next;
    struct ftable_file *prev;
};

// A bucket in the ftable. This map resolves collisions
// with chaining.
struct ftable_bucket {
    // Doubly-LL of collisions
    struct ftable_file *head;
    struct ftable_file *tail;
    size_t n_entries;
};

// A map between ftable_keys (filenames) to ftable_entries (a file).
// A file is just a size and an offset, which is what an
// ftable_entry is. struct file is not involved in this at all.
struct ftable {
    struct ftable_bucket *buckets[NUM_BUCKETS];
    size_t n_files;
};

struct ftable *new_ftable();
struct ftable_file *new_ftable_file(char name[], size_t s, size_t offset);

// Add a file to the ftable
unsigned int ftable_add_file(
    struct ftable *ft, char name[],
    size_t s, size_t offset
);

struct ftable_file ftable_get_file(struct ftable *ft, char name[]);

int print_ftable(struct ftable *ft);
void destroy_ftable(struct ftable *ftable);

#endif
