#include "fs.h"

void test_fs()
{
	struct fs *tfs = new_fs(); // Make the fs
	struct file tfile = new_file("testfile.txt"); // Make a new file

	fs_add_file(tfs, tfile, 60); // Add the file to the fs
	mem_dump(tfs->mem, HEX);
	
	// Test mem reading
	uint8_t *some_read = mem_read(tfs->mem, 6, 62);
	printf("random read: ");
	for (int i = 0; i < 6; i++) {
        printf("%2x ", some_read[i]);
	}
	printf("\n");

    // Get a file    
	struct file file_read = fs_get_file(tfs, "testfile.txt");
    print_file(file_read);

    free(some_read);
    free(file_read.bytes);
    destroy_fs(tfs);
    destroy_file(tfile);
}

void get_fs_(struct fs *tfs, char fname[])
{
    printf("\n\n");
    struct file tread = fs_get_file(tfs, fname);
    print_file(tread);
    for (int i = 0; i < tread.s; i++)
        printf("%c", tread.bytes[i]);
    printf("\n");
    destroy_file(tread);
    printf("\n\n");
}

void test_fs_simple()
{
    struct fs *tfs = new_fs();
    struct file tfile1 = new_file("testfile.txt");
    struct file tfile2 = new_file("testfile2.txt");
    struct file tfile3 = new_file("Makefile");

    fs_add_file(tfs, tfile2, 0);
    fs_add_file(tfs, tfile1, 100);
    fs_add_file(tfs, tfile3, 200);
    fs_add_file(tfs, tfile3, 600);

    mem_dump(tfs->mem, HEX);

    get_fs_(tfs, "filename");
    get_fs_(tfs, "testfile.txt");
    get_fs_(tfs, "testfile2.txt");
    get_fs_(tfs, "Makefile");

    destroy_fs(tfs);

    destroy_file(tfile1);
    destroy_file(tfile2);
    destroy_file(tfile3);
}

void get(struct ftable *ft, char name[])
{
    struct ftable_file test_get = ftable_get_file(ft, name);
    printf("name: %s\n  s: %zu\n  offset: %zu\n\n", test_get.name, test_get.s, test_get.offset);
}

void test_ft()
{
    struct ftable *ft = new_ftable();
    unsigned long key = ftable_add_file(ft, "fname", 10, 20);
    printf("key: %lu\n", key);

    get(ft, "name");
    get(ft, "fname");

    unsigned long key1 = ftable_add_file(ft, "abcdef", 33, 44);
    unsigned long key2 = ftable_add_file(ft, "fdebga", 55, 66);
    unsigned long key3 = ftable_add_file(ft, "awesome", 77, 88);
    printf("keys: %zu, %zu, %zu\n", key1, key2, key3);

    get(ft, "abcdef");
    get(ft, "fdebga");
    get(ft, "not");
    get(ft, "awesome");

    printf("testing bucket_get_file_index\n");
    for (int k = 0; k < NUM_BUCKETS; k++) {
        printf("n files: %ld\n", ft->buckets[k]->n_entries);
        for (int j = 0; j < ft->buckets[k]->n_entries; j++) {
            struct ftable_file ftf = bucket_get_file_index(
                ft->buckets[k], j
            );
            printf("name: %s\n  s: %zu\n  offset: %zu\n\n", ftf.name, ftf.s, ftf.offset);
        }
    }

    destroy_ftable(ft);

}

int test_mem()
{
	return 0;
}

int main()
{
    test_fs();
    test_fs_simple();
    test_mem();
    test_ft();

    return 0;
}

