#include "net/sdb.h"

static struct fs get_temp_fs()
{
    struct fs *tfs = new_fs();
    struct file tfile1 = new_file("files/testfile.txt");
    fs_add_file(tfs, tfile1, 0);
    destroy_file(tfile1);
    struct fs fs = *tfs;
    destroy_fs(tfs);
    return fs;
}

int test_put()
{
    server_db *sdb = init_sdb();
    struct fs fs = get_temp_fs();
    sdb_put_fs(sdb, fs);
    sdb_list_fsids(sdb);

    destroy_sdb(sdb);

    return 0;
}

int test_get()
{
    server_db *sdb = init_sdb();
    sdb_list_fsids(sdb);

    // Get temp fs
    struct fs *tfs = new_fs();
    struct file tfile1 = new_file("files/testfile.txt");
    fs_add_file(tfs, tfile1, 0);
    destroy_file(tfile1);
    struct fs fs = *tfs;
    destroy_fs(tfs);

    // Put temp fs
    sdb_put_fs(sdb, fs);
    sdb_list_fsids(sdb);

    // // Fetch it again
    struct fs fetched = sdb_get_fs(sdb, fs.fsid); // BUG is here
    char *sfsid = stringify_fsid(fs.fsid);
    printf("[fetched fsid]  %s\n", sfsid);
    printf("[fetched vdisk]\n");
    vdisk_dump(fetched.disk, HEX, DISK_SIZE);
    free(sfsid);
    
    // fs_list_files(fs); 

    destroy_sdb(sdb);
    return 0;
}

int main()
{
    int status = 0;
    //status = test_put();
    status = test_get();

    return status;
}

