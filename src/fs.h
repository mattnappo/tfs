#ifndef FS_H
#define FS_H

#include "vdisk.h"
#include "ftable.h"
#include "crypto.h"

#include <time.h>

#define FSID_LEN HASH_LEN
#define PROTO_PADDING 128
#define MAX_FILE_LEN (5000-PROTO_PADDING) // 5 KB

// A frontend, end-user-ready file. struct fs does not contain this.
struct file {
    char name[FILENAME_SIZE];
    uint8_t *bytes;
    size_t s;
};

// The main filesystem
struct fs {
    struct vdisk *disk;
    struct ftable *ft;
};

struct temp_fsid {
    uint8_t fsid[FSID_LEN];
};

/* fs methods */
struct fs*  new_fs         ();
int         fs_add_file    (struct fs *fs, struct file f, size_t offset);
struct file fs_get_file    (struct fs *fs, char *name);
void        fs_list_files  (struct fs  fs);
int         fs_remove_file (struct fs *fs, struct file f);
void        fs_print_file  (struct fs *fs, char *name, int show_mem);
void        destroy_fs     (struct fs *fs);

/* fsid methods */
struct temp_fsid calc_fsid(struct fs *fs);
void print_fsid(uint8_t fsid[FSID_LEN]);
char *stringify_fsid(uint8_t fsid[FSID_LEN]);

/* front end file functions */
struct file new_file     (const char *name);
void        print_file   (struct file f, enum print_mode mode);
void        destroy_file (struct file f);
int         is_valid_filename(const char *name);

#endif

