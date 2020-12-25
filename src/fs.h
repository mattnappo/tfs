#ifndef FS_H
#define FS_H

#include "memory.h"
#include "ftable.h"

// A struct representing a discrete file independent of
// any other structure. A struct file is the frontend,
// it just provides users with a bundle of a file, its name,
// size, and actual data.
struct file {
    char name[FILENAME_SIZE];
    uint8_t *bytes;
    size_t s;
};

// A filesystem. Contains the memory itself, and the table
// of files.
struct fs {
    struct memory *mem;
    struct ftable *ft;
};

// Make a new fs
struct fs *new_fs();

// Add a file to an fs. TODO:
// In the future, this will not take an offset. It will find the offset.
void add_file(struct fs *fs, struct file f, size_t offset);

// Print the contents of a file.
void print_file(struct file f);

// Remove a file from an fs
int remove_file(struct fs *fs, struct file f);

// Get a file from an fs
struct file get_file(struct fs *fs, char name[]);

// Construct a file given a filename
struct file new_file(const char *name);

void destroy_file(struct file f);
void destroy_fs(struct fs *fs);

#endif

