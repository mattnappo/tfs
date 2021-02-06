#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#ifndef VDISK_H
#define VDISK_H

//#define DISK_SIZE 5000000 // 5 MB
#define DISK_SIZE 2000

enum print_mode {
    HEX,
    ASCII
};

struct vdisk {
    uint8_t *bytes;
    size_t s;
};

struct vdisk *new_vdisk();
void     destroy_vdisk (struct vdisk *disk);
int      vdisk_dump    (struct vdisk *disk, enum print_mode m, int endpoint);
int      vdisk_write   (struct vdisk *disk, uint8_t *b, size_t s, size_t offset);
uint8_t* vdisk_read    (struct vdisk *disk, size_t s, size_t offset);

#endif
