#include "serialize.h"
#include <stdio.h>
#include <assert.h>

void get(struct ftable *ft, char name[])
{
    struct ftable_file test_get = ftable_get_file(ft, name);
    printf("name: %s\n  s: %zu\n  offset: %zu\n\n", test_get.name, test_get.s, test_get.offset);
}

void test_file()
{
    struct file nf = new_file("files/newtestfile");
    uint8_t *buf;
    unsigned len = serialize_file(&buf, nf);
    destroy_file(nf);

    struct file df = deserialize_file(buf, len);
    if (df.s <= 0) {
        printf("error deserializing\n");
        return;
    }

    print_file(df, ASCII);
    destroy_file(df);
    free(buf);
}

void test_vdisk()
{
    // Make a vdisk
    uint8_t *s = (uint8_t *)"test bytes\0";

    struct vdisk *vdisk = new_vdisk();
    vdisk_write(vdisk, s, 10, 0);

    // Serialize
    uint8_t *serialized_buf;
    // Returns the length of the serialized bytes
    unsigned len = serialize_vdisk(&serialized_buf, vdisk);

    // Deserialize into a new vdisk*
    struct vdisk *d_vdisk = deserialize_vdisk(serialized_buf, len);

    vdisk_dump(d_vdisk, HEX, 2000);
    destroy_vdisk(vdisk);
    destroy_vdisk(d_vdisk);
}

void test_ftfile()
{
    char *filename = "filename";
    struct ftable_file *ftf = new_ftable_file(filename, 20, 30);
    uint8_t *serialized;
    unsigned len = serialize_ftable_file(&serialized, ftf);
    for (int u = 0; u < len; u++)
        printf("%0x ", serialized[u]);
    printf("\n");
  
   struct ftable_file *deserialized = deserialize_ftable_file(serialized, len);
   printf("deserialized:\n  name: %s\n  size: %ld\noffset: %ld\n",
       deserialized->name, deserialized->s, deserialized->offset
   );

   destroy_ftable_file(ftf);
   destroy_ftable_file(deserialized);
}

void test_ftbucket()
{
    // unsigned serialize_ftable_bucket(uint8_t **buf, struct ftable_bucket *bucket)
    struct ftable *ft = new_ftable();
    ftable_add_file(ft, "fname", 10, 20);
    ftable_add_file(ft, "abcdef",  33, 44);
    ftable_add_file(ft, "fdebga",  55, 66);
    ftable_add_file(ft, "awesome", 77, 88);

    for (int k = 0; k < NUM_BUCKETS; k++) {
        if (ft->buckets[k]->n_entries > 0) {
            // Serialize
            uint8_t *buffer;
            unsigned blen = serialize_ftable_bucket(&buffer, ft->buckets[k]);
            printf("bucket: \n  ");
            for (int u = 0; u < blen; u++)
                printf("%x ", buffer[u]);
            printf("\n");

            // Deserialize
            struct ftable_bucket *deserialized =
                deserialize_ftable_bucket(buffer, blen);

            struct ftable_file *temp = deserialized->head;
            while (temp != NULL) {
                print_ftable_file(*temp);
                temp = temp->next;
            }

            destroy_ftable_file(temp);
            destroy_ftable_bucket(deserialized);
            free(buffer);
        }
    }

    destroy_ftable(ft);
}

void test_ftable()
{
    // unsigned serialize_ftable_bucket(uint8_t **buf, struct ftable_bucket *bucket)
    struct ftable *ft = new_ftable();
    ftable_add_file(ft, "fname", 10, 20);
    ftable_add_file(ft, "abcdef",  33, 44);
    ftable_add_file(ft, "fdebga",  55, 66);
    ftable_add_file(ft, "awesome", 77, 88);

    // serialize
    uint8_t *buffer;
    unsigned slen = serialize_ftable(&buffer, ft);

    // deserialize
    struct ftable *dft = deserialize_ftable(buffer, slen);

    // print deserialized
    printf("deserialized: \n");
    for (int k = 0; k < NUM_BUCKETS; k++) {
        if (dft->buckets[k]->n_entries > 0) {
            struct ftable_file *temp = dft->buckets[k]->head;
            while (temp != NULL) {
                print_ftable_file(*temp);
                temp = temp->next;
            }
            destroy_ftable_file(temp);
        }
    }

    destroy_ftable(dft);

    destroy_ftable(ft);
    free(buffer);
}

void test_fs()
{
    struct fs *tfs = new_fs();
    struct file tfile1 = new_file("files/testfile.txt");
    struct file tfile2 = new_file("files/testfile2.txt");
    struct file tfile3 = new_file("files/test_file");

    fs_add_file(tfs, tfile2, 0);
    fs_add_file(tfs, tfile1, 100);
    fs_add_file(tfs, tfile3, 200);
    fs_add_file(tfs, tfile3, 600);

    vdisk_dump(tfs->disk, HEX, 2000);
    print_ftable(*tfs->ft);

    get(tfs->ft, "files/filename");
    get(tfs->ft, "files/testfile.txt");
    get(tfs->ft, "files/testfile2.txt");
    get(tfs->ft, "files/test_file");

    // Serialize
    uint8_t *buffer;
    unsigned slen = serialize_fs(&buffer, tfs);

    // Deserialize
    struct fs *deserialized = deserialize_fs(buffer, slen);
    vdisk_dump(deserialized->disk, HEX, 2000);
    print_ftable(*deserialized->ft);

    get(deserialized->ft, "files/filename");
    get(deserialized->ft, "files/testfile.txt");
    get(deserialized->ft, "files/testfile2.txt");
    get(deserialized->ft, "files/test_file");
    destroy_fs(deserialized);

    free(buffer);

    destroy_file(tfile1);
    destroy_file(tfile2);
    destroy_file(tfile3);
    destroy_fs(tfs);
}

void test_io()
{
    struct fs *tfs = new_fs();
    struct file tfile1 = new_file("files/testfile.txt");
    struct file tfile2 = new_file("files/testfile2.txt");
    struct file tfile3 = new_file("files/test_file");

    fs_add_file(tfs, tfile1, 0);
    fs_add_file(tfs, tfile2, 100);
    fs_add_file(tfs, tfile3, 200);

    destroy_file(tfile1);
    destroy_file(tfile2);
    destroy_file(tfile3);

    get(tfs->ft, "files/testfile.txt");
    get(tfs->ft, "files/testfile2.txt");
    get(tfs->ft, "files/test_file");

    int status = write_fs("files/test_fs.fs", tfs);
    printf("fs write status: %d\n", status);
    destroy_fs(tfs);

    struct fs *dfs2 = read_fs("files/test_fs.fs");
    get(dfs2->ft, "files/testfile.txt");
    get(dfs2->ft, "files/testfile2.txt");
    get(dfs2->ft, "files/test_file");

    destroy_fs(dfs2);
}

void test_fsid()
{
    struct fs *fs = read_fs("files/test_sdb.fs");
    destroy_fs(fs);
}

void gen_sdb_fs()
{
    struct fs *tfs = new_fs();
    struct file tfile1 = new_file("files/testfile.txt");
    struct file tfile2 = new_file("files/testfile2.txt");

    fs_add_file(tfs, tfile1, 0);
    fs_add_file(tfs, tfile2, 100);

    destroy_file(tfile1);
    destroy_file(tfile2);
    write_fs("files/test_sdb.fs", tfs);
    destroy_fs(tfs);
}

int main()
{
    //test_file();
    //test_vdisk();
    //test_ftfile();
    //test_ftbucket();
    //test_ftable();
    //test_fs();
    //test_io();
    test_fsid();
    gen_sdb_fs();

    return 0;
}
