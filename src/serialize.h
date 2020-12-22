#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "proto/build/tfs.pb-c.h"
#include "fs.h"

#define MAX_MEM_SIZE MEM_SIZE+32

/* serialize the given pointer into buf */
unsigned serialize_memory        (uint8_t **buf, struct memory *mem);
unsigned serialize_ftable_file   (uint8_t **buf, struct ftable_file *file);
unsigned serialize_ftable_bucket (uint8_t **buf, struct ftable_bucket *bucket);
unsigned serialize_ftable        (uint8_t *buf, struct ftable *ft);

/* deserialize the given buffer and return a pointer to that type */
struct memory      *deserialize_memory      (uint8_t *buf, unsigned len);
struct ftable_file *deserialize_ftable_file (uint8_t *buf, unsigned len);
unsigned deserialize_ftable_bucket (struct ftable_bucket *bucket, uint8_t *buf);
unsigned deserialize_ftable        (struct ftable *ft,            uint8_t *buf);

#endif
