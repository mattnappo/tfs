#ifndef FS_H
#define FS_H

#include "memory.h"
#include "ftable.h"

// A frontend, end-user-ready file. struct fs does not contain this.
struct file {
    char name[FILENAME_SIZE];
    uint8_t *bytes;
    size_t s;
};

// The main filesystem
struct fs {
    struct memory *mem;
    struct ftable *ft;
};

/* fs methods */
struct fs *new_fs();
void fs_add_file(struct fs *fs, struct file f, size_t offset);
struct file fs_get_file(struct fs *fs, char *name);
int  fs_remove_file(struct fs *fs, struct file f);
void fs_print_file(struct fs *fs, char *name, int show_mem);
void destroy_fs(struct fs *fs);

/* front end file functions */
void print_file(struct file f);
struct file new_file(const char *name);
void destroy_file(struct file f);

#endif

