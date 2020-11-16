#ifndef FTABLE_H
#define FTABLE_H

#include <stdlib.h>
#include <math.h>
#include "util.h"

// A power of 2
#define NUM_BUCKETS 16

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
    struct ftable_file *entries; // Doubly-LL of collisions
    size_t n_entries;
};

// An ftable_keys (filenames) to ftable_entries (a file).
// A file is just a size and an offset, which is what an
// ftable_entry is. struct file is not involved in this at all.
struct ftable {
    struct ftable_bucket *bucekts[NUM_BUCKETS];
    size_t f_count;
};

// The key space of an ftable
struct ftable_key {
    char name[FILENAME_SIZE]; // The name of the file
};

// Construct a new ftable
struct ftable *new_ftable();

// Add a file entry into the ftable
int add_ftable_entry(
    struct ftable *ft,
    struct ftable_key *key,
    struct ftable_entry *entry
);

// Print the files in a ftable
int print_ftable(struct ftable *ft);
void destroy_ftable(struct ftable *ftable);

#endif
