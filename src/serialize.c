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

struct memory *deserialize_memory(uint8_t *buf, unsigned len)
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

unsigned serialize_ftable_file(uint8_t **buf, struct ftable_file *file)
{
    FiletableFile ftf = FILETABLE_FILE__INIT;
    ftf.name = malloc(strlen(file->name) + 1);
    strcpy(ftf.name, file->name);
    ftf.s = file->s;
    ftf.offset = file->offset;

    unsigned len = filetable_file__get_packed_size(&ftf);
    *buf = malloc(len);
    filetable_file__pack(&ftf, *buf);
    printf("writing %d serialized bytes\n", len);
    free(ftf.name);
    return len;
}

struct ftable_file *deserialize_ftable_file(uint8_t *buf, unsigned len)
{
    FiletableFile *ftf = filetable_file__unpack(NULL, len, buf);
    if (ftf == NULL) {
        printf("error unpacking bytes.\n");
        return NULL;
    }

    struct ftable_file *ftfile = new_ftable_file(ftf->name, ftf->s, ftf->offset);
    filetable_file__free_unpacked(ftf, NULL);
    free(buf);
    return ftfile;
}

unsigned serialize_ftable_bucket(uint8_t **buf, struct ftable_bucket *bucket)
{
    FiletableBucket ftb = FILETABLE_BUCKET__INIT;
    FiletableFile **files;

    files = malloc(sizeof(FiletableBucket *) * bucket->n_entries);
    for (int i = 0; i < bucket->n_entries; i++) {
        files[i] = malloc(sizeof(FiletableFile));
        filetable_file__init(files[i]);
        struct ftable_file fetched = bucket_get_file_index(bucket, i);
        files[i]->name = malloc(strlen(fetched.name) + 1);
        strcpy(files[i]->name, fetched.name);
        files[i]->s = fetched.s;
        files[i]->offset = fetched.offset;
    }

    ftb.n_files = bucket->n_entries;
    ftb.files = files;
    unsigned len = filetable_bucket__get_packed_size(&ftb);
    *buf = malloc(len);
    filetable_bucket__pack(&ftb, *buf);
    fprintf(stderr, "writing %d serialized bytes\n", len);

    for (int i = 1; i < bucket->n_entries; i++) {
        free(files[i]->name);
        free(files[i]);
    }
    free(ftb.files);
    free(files);
    return len;
}

unsigned deserialize_ftable_bucket(struct ftable_bucket *bucket, uint8_t *buf);

unsigned serialize_ftable(uint8_t *buf, struct ftable *ft);
unsigned deserialize_ftable(struct ftable *ft, uint8_t *buf);

