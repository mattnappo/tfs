#include "sdb.h"

#define A ((sqrt(5) - 1) / 2)

fsdb *init_fsdb()
{
    fsdb *fdb = malloc(sizeof(fsdb));
    // Init the lock
    pthread_mutex_t *lock = malloc(sizeof(pthread_mutex_t));
    if (pthread_mutex_init(lock, NULL) != 0) {
        printf("failed to init fsdb lock\n");
        return NULL;
    }
    fdb->lock = lock;

    // Make the map
    fdb->n_fs = 0;
    for (int i = 0; i < FSDB_BUCKETS; i++) {
        // Create a new bucket
        struct fsdb_bucket *bucket = malloc(sizeof(struct fsdb_bucket));
        bucket->head = NULL;
        bucket->tail = NULL;
        bucket->n_entries = 0;
        fdb->buckets[i] = bucket;
    }

    return fdb;
}

static uint64_t fsdb_hash(uint8_t fsid[FSID_LEN])
{
    // Because the fsid is itself already an md5 hash, there is no need
    // to actually generate a hash code. Just piece the first few bytes together.
    uint64_t code = 0;
    for (int i = 0; i < FSID_LEN / 2; i++) // Little endian concatenation
        code |= (fsid[i] << i*8);
    return code;
}

// Get bucket from fsid
static struct fsdb_bucket *fsdb_get_bucket(fsdb *fdb, uint8_t fsid[])
{
    return fdb->buckets[fsdb_hash(fsid) % FSDB_BUCKETS];
}

static int fs_in_fsdb(fsdb *fdb, uint8_t fsid[FSID_LEN])
{
    struct fsdb_bucket *bucket = fsdb_get_bucket(fdb, fsid);
    struct fsdb_fs *temp = bucket->head;
    while (temp != NULL) {
        if (fsid_equal(fsid, temp->fsid))
            return 1;
        temp = temp->next;
    }
    return 0;
}

static struct fsdb_fs *new_fsdb_fs(struct fs *fs, uint8_t fsid[FSID_LEN])
{
    struct fsdb_fs *entry = malloc(sizeof(struct fsdb_fs));
    memcpy(entry->fsid, fsid, FSID_LEN);
    entry->next = NULL;
    entry->prev = NULL;
    entry->fs = fs;
    return entry;
}

static void fsdb_add_entry(struct fsdb_bucket *bucket, struct fsdb_fs *entry)
{
    if (bucket->head == NULL) {
        bucket->head = entry;
        bucket->n_entries++;
        return;
    }

    // TODO: Make this a forward insertion, not an append
    struct fsdb_fs *temp = bucket->head;
    while (temp->next != NULL)
        temp = temp->next;
    temp->next = entry;
    temp->prev = temp;
    bucket->tail = temp->next;
    bucket->n_entries++;
}

/* ----- PUBLIC METHODS ----- */

server_db *init_sdb()
{
    server_db *sdb = malloc(sizeof(server_db));
    sdb->n_connections = 0;
    sdb->fdb = init_fsdb();

    // Load each fs on disk
    // For now, just load one
    // Also eventually make a flag to init, or load. Have multiple fsdbs on disk
    // naming somehow.
    struct fs *fs = read_fs("files/test_fs.fs");
    if (fs == NULL) {
        printf("unable to load fs from disk.\n");
        return NULL;
    }
    fs_list_files(*fs);
    if (sdb_put_fs(sdb, fs) != 0) {
        printf("unable to insert fs into fsdb.\n");
        return NULL;
    }
    return sdb;
}

struct fs sdb_get_fs(server_db *sdb, uint8_t fsid[FSID_LEN])
{
    struct fsdb_bucket *bucket = fsdb_get_bucket(sdb->fdb, fsid);
    struct fsdb_fs *temp = bucket->head;
    while (temp != NULL) {
        if (fsid_equal(fsid, temp->fsid))
            return *temp->fs;
        temp = temp->next;
    }
    char *sfsid = stringify_fsid(fsid);
    printf("fsid %s not in fsdb.\n", sfsid);
    free(sfsid);
    return (struct fs) {};
}

int sdb_put_fs(server_db *sdb, struct fs *fs)
{
    struct temp_fsid temp_fsid = calc_fsid(fs);
    uint8_t fsid[FSID_LEN];
    memcpy(fsid, temp_fsid.fsid, FSID_LEN);
    if (fs_in_fsdb(sdb->fdb, fsid) == 1) {
        char *sfsid = stringify_fsid(fsid);
        printf("fs %s already in the fsdb.\n", sfsid);
        free(sfsid);
        return 1;
    }
    struct fsdb_fs *entry = new_fsdb_fs(fs, fsid);
    struct fsdb_bucket *bucket = fsdb_get_bucket(sdb->fdb, fsid);
    fsdb_add_entry(bucket, entry);
    sdb->fdb->n_fs++;
    char *sfsid = stringify_fsid(fsid);
    printf("put fsid %s in fsdb.\n", sfsid);
    free(sfsid);
    return 0;
}

void sdb_list_fsids(server_db *sdb)
{
    printf("===== BEGIN FSDB DUMP =====\n");
    for (int i = 0; i < FSDB_BUCKETS; i++) {
        struct fsdb_fs *temp = sdb->fdb->buckets[i]->head;
        while (temp != NULL) {
            char *fsid = stringify_fsid(temp->fsid);
            printf("%s\n", fsid);
            free(fsid);
            temp = temp->next;
        }
    }
    printf("===== END FSDB DUMP =====\n");
}

void destroy_fsdb_fs(struct fsdb_fs *entry)
{
    destroy_fs(entry->fs);
    free(entry);
}

void destroy_fsdb_bucket(struct fsdb_bucket *bucket)
{
    struct fsdb_fs *temp;
    while (bucket->head != NULL) {
        temp = bucket->head;
        bucket->head = bucket->head->next;
        destroy_fsdb_fs(temp);
    }
    free(bucket);
}

void destroy_fsdb(fsdb *fdb)
{
    for (int i = 0; i < FSDB_BUCKETS; i++)
        destroy_fsdb_bucket(fdb->buckets[i]);
    free(fdb->lock);
    free(fdb);
}

void destroy_sdb(server_db *sdb)
{
    destroy_fsdb(sdb->fdb);
    free(sdb);
}
