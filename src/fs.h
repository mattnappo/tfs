#include <string.h>
#include "memory.h"

#ifndef FS_H
#define FS_H

#define FILENAME_SIZE 16
#define MAX_FILES     16

// A struct representing a discrete file independent of
// any other structure.
struct file {
    char name[FILENAME_SIZE];
    char *bytes;
    size_t s;
};

// An entry into a ftable->files.
struct ftable_entry {
    char name[FILENAME_SIZE];
    size_t s;
    size_t offset;
};

// An array that holds the metadata of each file.
struct ftable {
    struct ftable_entry *files[MAX_FILES];
    size_t f_count;
};

// A filesystem. The main struct.
struct fs {
    struct memory *mem;
    struct ftable *ft;
};

// Make a new fs
struct fs *new_fs();

// Add a new file to an fs
int add_file(struct fs *fs, struct file *f, size_t offset);

// Print the contents of a file.
void print_file(struct file f);

// Remove a file from an fs
int remove_file(struct fs *fs, struct file *f);

// Get a file from an fs
struct file *get_file(struct fs *fs, size_t findex);

// Construct a file given a filename
struct file *new_file(const char *name);

// Construct a new ftable
struct ftable *new_ftable();

// Add a file entry into the ftable
int add_ftable_entry(struct ftable *ft, struct ftable_entry *entry);

// Print the files in a ftable
int print_ftable(struct ftable *ft);

#endif

