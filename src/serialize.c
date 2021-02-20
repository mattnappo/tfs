#include "serialize.h"
#include "util.h"

typedef enum serialize_error_ {
    ERR_DESERIALIZE = 1
} serialize_error;


unsigned serialize_file(uint8_t **buf, struct file file)
{
    File f = FILE__INIT;
    f.name = file.name;
    f.bytes_.len = file.s;
    f.bytes_.data = malloc(file.s);
    memcpy(f.bytes_.data, file.bytes, file.s);
    unsigned len = file__get_packed_size(&f);
    *buf = malloc(len);
    file__pack(&f, *buf);
    free(f.bytes_.data);
    return len;
}

struct file deserialize_file(uint8_t *buf, unsigned len)
{
    File *f = file__unpack(NULL, len, buf);
    if (f == NULL) {
        printf("error unpacking bytes.\n");
        struct file ef = {};
        return ef;
    }

    struct file file = { .s = f->bytes_.len };
    strcpy(file.name, f->name);
    file.bytes = malloc(file.s);
    memcpy(file.bytes, f->bytes_.data, f->bytes_.len);
    file__free_unpacked(f, NULL);
    return file;
}

unsigned serialize_vdisk(uint8_t **buf, struct vdisk *disk)
{
    VDisk vdisk_b = VDISK__INIT;
    vdisk_b.bytes_.data = malloc(disk->s);
    vdisk_b.bytes_.len = disk->s;
    memcpy(vdisk_b.bytes_.data, disk->bytes, disk->s);
    unsigned len = vdisk__get_packed_size(&vdisk_b);
    *buf = malloc(len);
    vdisk__pack(&vdisk_b, *buf);
    printf("writing %d serialized bytes\n", len);
    free(vdisk_b.bytes_.data);
    return len;
}

struct vdisk *deserialize_vdisk(uint8_t *buf, unsigned len)
{
    VDisk *vdisk_b = vdisk__unpack(NULL, len, buf);
    if (vdisk_b == NULL) {
        printf("error unpacking bytes.\n");
        return NULL;
    }

    struct vdisk *vdisk = new_vdisk();
    memcpy(vdisk->bytes, vdisk_b->bytes_.data, vdisk_b->bytes_.len);
    vdisk->s = vdisk_b->bytes_.len;
     
    vdisk__free_unpacked(vdisk_b, NULL);
    free(buf);
    return vdisk;
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
    Filesystem sfs   = FILESYSTEM__INIT;
    VDisk      sdisk = VDISK__INIT;
    Filetable  sft   = FILETABLE__INIT;

    // Make the vdisk
    sdisk.bytes_.data = malloc(fs->disk->s);
    sdisk.bytes_.len = fs->disk->s;
    memcpy(sdisk.bytes_.data, fs->disk->bytes, fs->disk->s);

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

    // Copy over the fsid (do not calculate it)
    sfs.fsid.data = malloc(FSID_LEN);
    sfs.fsid.len = FSID_LEN;
    memcpy(sfs.fsid.data, fs->fsid, FSID_LEN);

    // Set into final fs to be serialized
    sfs.ft = &sft;
    sfs.disk = &sdisk;

    // Pack
    unsigned len = filesystem__get_packed_size(&sfs);
    if (len < MAX_PACKED_FS_LEN) {
        *buf = malloc(len);
        filesystem__pack(&sfs, *buf);
        printf("writing %d serialized bytes\n", len);
    } else {
        len = 0;
    }

    // Free vdisk, ftable, and fsid
    free(sdisk.bytes_.data);
    for (int j = 0; j < NUM_BUCKETS; j++) {
        for (int i = 0; i < buckets[j]->n_files; i++) {
            free(buckets[j]->files[i]->name);
            free(buckets[j]->files[i]);
        }
        free(buckets[j]->files);
        free(buckets[j]);
    }
    free(buckets);
    free(sfs.fsid.data);

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
    dfs->disk = new_vdisk();
    memcpy(dfs->disk->bytes, fs->disk->bytes_.data, fs->disk->bytes_.len);

    dfs->ft = new_ftable();

    dfs->ft->n_files = 0;
    for (int j = 0; j < NUM_BUCKETS; j++) {
        FiletableBucket ftb = *fs->ft->buckets[j]; // Current bucket
        for (int i = 0; i < ftb.n_files; i++) {
            struct ftable_file *ftf = new_ftable_file(
                ftb.files[i]->name,
                ftb.files[i]->s,
                ftb.files[i]->offset
            );
            add_file_to_bucket(ftf, dfs->ft->buckets[j]);
            dfs->ft->n_files++;
        }
    }
    memcpy(dfs->fsid, fs->fsid.data, FSID_LEN); // Copy fsid

    filesystem__free_unpacked(fs, NULL);
    return dfs;
}

int write_fs(const char *filename, struct fs *fs)
{
    uint8_t *buffer;
    unsigned slen = serialize_fs(&buffer, fs);

    FILE *fd = fopen(filename, "w");
    size_t len = fwrite(buffer, 1, slen, fd);
    fclose(fd);
    if (len != slen) {
        printf("error writing fs to file.\n");
        return -1;
    }
    free(buffer);
    return 0;
}

struct fs *read_fs(const char *filename)
{
    FILE *fd = fopen(filename, "r");
    uint8_t *buffer = calloc(MAX_IO_SIZE, 1);
    size_t rlen = fread(buffer, 1, MAX_IO_SIZE, fd); 
    struct fs *dfs = deserialize_fs(buffer, rlen);
    free(buffer);
    fclose(fd);
    return dfs;
}
