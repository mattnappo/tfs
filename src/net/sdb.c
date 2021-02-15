#include "sdb.h"

#define A ((sqrt(5) - 1) / 2)

fsdb *init_fsdb()
{
    fsdb *fsdb = malloc(sizeof(fsdb));
    // Init the lock
    pthread_mutex_t *lock = malloc(sizeof(pthread_mutex_t));
    if (pthread_mutex_init(lock, NULL) != 0) {
        printf("failed to init fsdb lock\n");
        return NULL;
    }
    fsdb->lock = lock;

    // Make the map
    fsdb->n_fs = 0;
    for (int i = 0; i < FSDB_BUCKETS; i++) {
        // Create a new bucket
        struct fsdb_bucket *bucket = malloc(sizeof(struct fsdb_bucket));
        bucket->head = NULL;
        bucket->tail = NULL;
        bucket->n_entries = 0;
        fsdb->buckets[i] = bucket;
    }

    // Load each fs on disk into memory

    return fsdb;
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
static struct fsdb_bucket *fsdb_get_bucket(fsdb *fsdb, uint8_t fsid[])
{
    return fsdb->buckets[fsdb_hash(fsid) % FSDB_BUCKETS];
}

static int fs_in_fsdb(fsdb *fsdb, uint8_t fsid[FSID_LEN])
{
    struct fsdb_bucket *bucket = fsdb_get_bucket(fsdb, fsid);
    struct fsdb_fs *temp = bucket->head;
    while (temp != NULL) {
        if (memcmp(fsid, temp->fsid, FSID_LEN) == 0)
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

server_db *init_sdb()
{

    // load fsdb

    return NULL;
}

struct fs *sdb_get_fs(server_db *sdb, uint8_t fsid[FSID_LEN])
{

    return NULL;
}

int sdb_put_fs(server_db *sdb, struct fs *fs)
{
    struct temp_fsid temp_fsid = calc_fsid(fs);
    uint8_t fsid[FSID_LEN];
    memcpy(fsid, temp_fsid.fsid, FSID_LEN);
    if (fs_in_fsdb(sdb->fsdb, fsid) == 1) {
        printf("fs 0x%*x already in the fsdb.\n", FSID_LEN, *fsid);
        return 1;
    }
    struct fsdb_fs *entry = new_fsdb_fs(fs, fsid);
    struct fsdb_bucket *bucket = fsdb_get_bucket(sdb->fsdb, fsid);
    fsdb_add_entry(bucket, entry);
    sdb->fsdb->n_fs++;
    return 0;
}

int destroy_sdb(server_db *sdb)
{
    return 0;
}
