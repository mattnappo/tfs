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

    // Pack
    unsigned len = filetable__get_packed_size(&sft);
    *buf = malloc(len);
    filetable__pack(&sft, *buf);
    printf("writing %d serialized bytes\n", len);

    // Free
    for (int j = 0; j < NUM_BUCKETS; j++) {
        for (int i = 0; i < buckets[j]->n_files; i++) {
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
        FiletableBucket ftb = *ft->buckets[j]; // Current bucket
        for (int i = 0; i < ftb.n_files; i++) {
            struct ftable_file *ftf = new_ftable_file(
                ftb.files[i]->name,
                ftb.files[i]->s,
                ftb.files[i]->offset
            );
            add_file_to_bucket(ftf, new_bucket);
        }
        free(dft->buckets[j]); // Free what was already there
        dft->buckets[j] = new_bucket;
    }

    filetable__free_unpacked(ft, NULL);
    return dft;
}

unsigned serialize_fs(uint8_t **buf, struct fs *fs)
{
    Filesystem sfs  = FILESYSTEM__INIT;
    Memory     smem = MEMORY__INIT;
    Filetable  sft  = FILETABLE__INIT;

    // Make the memory
    smem.bytes_.data = malloc(fs->mem->s);
    smem.bytes_.len = fs->mem->s;
    memcpy(smem.bytes_.data, fs->mem->bytes, fs->mem->s);

    // Make the ftable
    FiletableBucket **buckets;
    buckets = malloc(sizeof(FiletableBucket *) * NUM_BUCKETS);
    for (int j = 0; j < NUM_BUCKETS; j++) {
        struct ftable_bucket *bucket = fs->ft->buckets[j]; // This is a temp
        buckets[j] = malloc(sizeof(FiletableBucket));
        filetable_bucket__init(buckets[j]);
        FiletableFile **files
            = malloc(sizeof(FiletableFile *) * bucket->n_entries);
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
    sft.n_files = fs->ft->n_files;

    // Set into final fs to be serialized
    sfs.ft = &sft;
    sfs.mem = &smem;

    // Pack
    unsigned len = filesystem__get_packed_size(&sfs);
    *buf = malloc(len);
    filesystem__pack(&sfs, *buf);
    printf("writing %d serialized bytes\n", len);

    // Free memory and ftable
    free(smem.bytes_.data);
    for (int j = 0; j < NUM_BUCKETS; j++) {
        for (int i = 0; i < buckets[j]->n_files; i++) {
            free(buckets[j]->files[i]->name);
            free(buckets[j]->files[i]);
        }
        free(buckets[j]->files);
        free(buckets[j]);
    }
    free(buckets);

    return len;
}

struct fs *deserialize_fs(uint8_t *buf, unsigned len)
{
    Filesystem *fs = filesystem__unpack(NULL, len, buf);
    if (fs == NULL) {
        printf("error unpacking bytes.\n");
        return NULL;
    }

    struct fs *dfs = malloc(sizeof(struct fs));
    dfs->mem = new_memory();
    memcpy(dfs->mem->bytes, fs->mem->bytes_.data, fs->mem->bytes_.len);

    dfs->ft = new_ftable();

    dfs->ft->n_files = 0;
    for (int j = 0; j < NUM_BUCKETS; j++) {
        FiletableBucket ftb = *fs->ft->buckets[j]; // Current bucket
        // dfs->ft->buckets[j] = new_ftable_bucket();
        for (int i = 0; i < ftb.n_files; i++) {
            struct ftable_file *ftf = new_ftable_file(
                ftb.files[i]->name,
                ftb.files[i]->s,
                ftb.files[i]->offset
            );
            add_file_to_bucket(ftf, dfs->ft->buckets[j]);
            dfs->ft->n_files++;
        }
        // free(dft->buckets[j]); // Free what was already there
        // dft->buckets[j] = malloc(sizeof(struct ftable_bucket));
        // dft->buckets[j] = new_bucket;
    }
    // destroy_ftable(dft);

    // dfs->ft = dft;
    // dfs->ft = malloc(sizeof(struct ftable));
    // dfs->ft = dft;
    // struct fs *dfs = new_fs();
    //dfs
    // dfs->mem = new_memory();
    // memcpy(dfs->mem->bytes, fs->mem->bytes_.data, fs->mem->bytes_.len);

    // dfs->ft = dft;
    // // free(dfs->mem);
    // dfs->mem = malloc(sizeof(fs->mem->bytes_.len));
    // memcpy(dfs->mem, fs->mem->bytes_.data, fs->mem->bytes_.len);

    // destroy_ftable(dft);

    filesystem__free_unpacked(fs, NULL);
    return dfs;
    // return NULL;
}

