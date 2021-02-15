#include "sdb.h"

server_db *init_sdb()
{
    pthread_mutex_t *lock;
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init failed\n");
        return 1;
    }

    // init mutex
    // load fsdb

    return NULL;
}

struct fs *fs sdb_get_fs(server_db *sdb, uint8_t fsid[FSID_LEN])
{

    return NULL;
}

int destroy_sdb(server_db *sdb)
{
    return 0;
}
