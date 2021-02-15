#ifndef SDB_H
#define SDB_H

#include <inttypes.h>
#include <pthread.h>
#include "fs.h"

#define FSDB_BUCKETS 16

static struct fsdb_fs {
    struct fs *fs;        // The fs itself
    struct fsdb_fs *next; // Forward link
    struct fsdb_fs *prev; // Backward link
};

static struct fsdb_bucket {
    struct fsdb_fs *head;
    struct fsdb_fs *tail;
    size_t n_entries;
};

static typedef struct fsdb {
    pthread_mutex_t lock;
    struct fsdb_bucket *buckets[FSDB_BUCKETS];
    size_t n_fs; // Number of entries
} fsdb;

// The main server state struct
typedef struct server_database {
    uint8_t n_connections;
    fsdb *fsdb;
    // valid tokendb
    // all current client IP and connection data
    // logger
} server_db;

server_db *init_sdb();
struct fs *fs sdb_get_fs(server_db *sdb, uint8_t fsid[FSID_LEN]);
int           sdb_put_fs(server_db *sdb, struct fs *fs);
int destroy_sdb(server_db *sdb);

#endif
