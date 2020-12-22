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

    files = malloc(sizeof(FiletableFile *) * bucket->n_entries);
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
    printf("writing %d serialized bytes\n", len);

    for (int i = 0; i < bucket->n_entries; i++) {
        free(files[i]->name);
        free(files[i]);
    }
    free(files);
    return len;
}

struct ftable_bucket *deserialize_ftable_bucket(uint8_t *buf, unsigned len)
{
    FiletableBucket *ftb = filetable_bucket__unpack(NULL, len, buf);
    if (ftb == NULL) {
        printf("error unpacking bytes.\n");
        return NULL;
    }

    struct ftable_bucket *ftable_b = new_ftable_bucket();
    for (int i = 0; i < ftb->n_files; i++) {
        struct ftable_file *ftf = new_ftable_file(
            ftb->files[i]->name,
            ftb->files[i]->s,
            ftb->files[i]->offset
        );
        add_file_to_bucket(ftf, ftable_b);
    }
    filetable_bucket__free_unpacked(ftb, NULL);
    return ftable_b;
}
unsigned serialize_ftable(uint8_t **buf, struct ftable *ft)
{
    Filetable sft = FILETABLE__INIT;
    FiletableBucket **buckets;
    buckets = malloc(sizeof(FiletableBucket *) * NUM_BUCKETS);

    for (int j = 0; j < NUM_BUCKETS; j++) {
        struct ftable_bucket *bucket = ft->buckets[j]; // This is a temp

        buckets[j] = malloc(sizeof(FiletableBucket));
        filetable_bucket__init(buckets[j]);

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
        buckets[j]->n_files = bucket->n_entries;
        buckets[j]->files = files;
    }
    sft.n_buckets = NUM_BUCKETS;
    sft.buckets = buckets;
    sft.n_files = ft->n_files;

    // pack
    unsigned len = filetable__get_packed_size(&sft);
    *buf = malloc(len);
    filetable__pack(&sft, *buf);
    printf("writing %d serialized bytes\n", len);

    // free
    for (int j = 0; j < NUM_BUCKETS; j++) {
        for (int i = 0; i < buckets[j]->n_files; i++) {
            printf("running i [%d]\n", i);
            free(buckets[j]->files[i]->name);
            free(buckets[j]->files[i]);
        }
        free(buckets[j]->files);
        free(buckets[j]);
    }
    free(buckets);

    return len;
}

struct ftable *deserialize_ftable(uint8_t *buf, unsigned len)
{
    Filetable *ft = filetable__unpack(NULL, len, buf);
    if (ft == NULL) {
        printf("error unpacking bytes.\n");
        return NULL;
    }

    struct ftable *dft = new_ftable();
    for (int j = 0; j < NUM_BUCKETS; j++) {
        struct ftable_bucket *new_bucket = new_ftable_bucket();
        FiletableBucket ftb = *ft->buckets[j]; // current bucket
        for (int i = 0; i < ftb.n_files; i++) {
            struct ftable_file *ftf = new_ftable_file(
                ftb.files[i]->name,
                ftb.files[i]->s,
                ftb.files[i]->offset
            );
            add_file_to_bucket(ftf, new_bucket);
        }
        free(dft->buckets[j]); // free what was already there
        dft->buckets[j] = new_bucket;
    }

    filetable__free_unpacked(ft, NULL);
    return dft;
}

unsigned serialize_fs(uint8_t **buf, struct fs *fs)
{
/*
    Filesystem sfs  = FILESYSTEM__INIT;
    Submessage smem = MEMORY__INIT;
    Submessage sft  = FILETABLE__INIT;

    smem.mem = ft->mem;
    smem.bytes_.data = malloc(fs->mem->s);
    smem.bytes_.len = fs->mem->s;
    memcpy(smem.bytes_.data, fs->mem->bytes, fs->mem->s);





    msg.a = &sub1;               // Point msg.a to sub1 data




    void *buf;
    unsigned len;

    // NOTE: has_b is not required like amessage, therefore check for NULL on deserialze
    if (argc == 3) {
        sub2.value = atoi (argv[2]);
        msg.b = &sub2; // Point msg.b to sub2 data 
    }

    len = emessage__get_packed_size (&msg); // This is the calculated packing length
    buf = malloc (len);                     // Allocate memory
    emessage__pack (&msg, buf);             // Pack msg, including submessages

    fprintf(stderr,"Writing %d serialized bytes\n",len); // See the length of message
    fwrite (buf, len, 1, stdout);           // Write to stdout to allow direct command line piping

    free(buf); // Free the allocated serialized buffer
    return 0;
    */
    return 0;
}

struct fs *deserialize_fs(uint8_t *buf, unsigned len);

