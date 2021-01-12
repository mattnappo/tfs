#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "proto/build/tfs.pb-c.h"
#include "fs.h"

#define MAX_PACKED_FILE_LEN (MAX_FILE_LEN+PROTO_PADDING)
#define MAX_PACKED_FS_LEN   (DISK_SIZE+4096+(PROTO_PADDING*2))
#define MAX_IO_SIZE         MAX_PACKED_FS_LEN

/* serialize the given pointer into buf */
unsigned serialize_vdisk         (uint8_t **buf, struct vdisk *disk);
unsigned serialize_ftable_file   (uint8_t **buf, struct ftable_file *file);
unsigned serialize_ftable_bucket (uint8_t **buf, struct ftable_bucket *bucket);
unsigned serialize_ftable        (uint8_t **buf, struct ftable *ft);
unsigned serialize_fs            (uint8_t **buf, struct fs *fs);

/* deserialize the given buffer and return a pointer to that type */
struct vdisk         *deserialize_vdisk         (uint8_t *buf, unsigned len);
struct ftable_file   *deserialize_ftable_file   (uint8_t *buf, unsigned len);
struct ftable_bucket *deserialize_ftable_bucket (uint8_t *buf, unsigned len);
struct ftable        *deserialize_ftable        (uint8_t *buf, unsigned len);
struct fs            *deserialize_fs            (uint8_t *buf, unsigned len);

/* serialize to disk */
int        write_fs (const char *filename, struct fs *fs);
struct fs *read_fs  (const char *filename);

#endif
