#include "net/sdb.h"

int test_get_put()
{
    server_db *sdb = init_sdb();
    sdb_list_fsids(sdb); // Should be null

    // Generate a temp fs
    struct fs *tmp_fsp = new_fs();
    struct file tmp_f = new_file("files/testfile.txt");
    fs_add_file(tmp_fsp, tmp_f, 0);
    destroy_file(tmp_f);

    // Put temp fs
    sdb_put_fs(sdb, *tmp_fsp);
    sdb_list_fsids(sdb);

    // Fetch it again
    struct fs fetched = sdb_get_fs(sdb, tmp_fsp->fsid);
    char *sfsid = stringify_fsid(fetched.fsid);
    printf("[fetched fsid]  %s\n", sfsid);
    printf("[fetched vdisk]\n");
    vdisk_dump(fetched.disk, HEX, 500);
    free(sfsid);
    
    fs_list_files(fetched); 

    destroy_sdb(sdb);
    destroy_fs(tmp_fsp);
    return 0;
}

int main()
{
    int status = 0;
    status = test_get_put();

    return status;
}

