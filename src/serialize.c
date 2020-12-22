#include "serialize.h"
#include "util.h"

typedef enum serialize_error_ {
    ERR_DESERIALIZE = 1
} serialize_error;

unsigned serialize_memory(uint8_t **buf, struct memory *mem)
{
    Memory mem_b = MEMORY__INIT;
    mem_b.bytes_.data = malloc(mem->s);
    mem_b.bytes_.len = mem->s;
    memcpy(mem_b.bytes_.data, mem->bytes, mem->s);
    unsigned len = memory__get_packed_size(&mem_b);
    *buf = malloc(len);
    memory__pack(&mem_b, *buf);
    printf("writing %d serialized bytes\n", len);
    free(mem_b.bytes_.data);
    return len;
}

struct memory *deserialize_memory(void *buf, unsigned len)
{
    Memory *mem_b = memory__unpack(NULL, len, buf);
    if (mem_b == NULL) {
        printf("error unpacking bytes.\n");
        return NULL;
    }

    struct memory *mem = new_memory();
    memcpy(mem->bytes, mem_b->bytes_.data, mem_b->bytes_.len);
    mem->s = mem_b->bytes_.len;
     
    memory__free_unpacked(mem_b, NULL);
    free(buf);
    return mem;
}

unsigned serialize_ftable_file(uint8_t *buf, struct ftable_file *file);
unsigned deserialize_ftable_file(struct ftable_file *file, uint8_t *buf);

unsigned serialize_ftable_bucket(uint8_t *buf, struct ftable_bucket *bucket);
unsigned deserialize_ftable_bucket(struct ftable_bucket *bucket, uint8_t *buf);

unsigned serialize_ftable(uint8_t *buf, struct ftable *ft);
unsigned deserialize_ftable(struct ftable *ft, uint8_t *buf);

