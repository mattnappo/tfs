#include "fs.h"

int test_fs()
{
	struct fs *tfs = new_fs(); // Make the fs
	struct file *tfile = new_file("testfile.txt"); // Make a new file

	int status;
	status = add_file(tfs, tfile, 60); // Add the file to the fs
	status = add_file(tfs, tfile, 0); // Add the file to the fs
	status = dump(tfs->mem, HEX);

	
	// Test mem reading
	char *some_read = read(tfs->mem, 15, 6);
	printf("random read: ");
	for (int i = 0; i < 15; i++) {
	    printf("%2x ", some_read[i]);
	}
	printf("\n");
	
	print_ftable(tfs->ft);

	struct file *file_read = get_file(tfs, 0);

	print_file(*file_read);

	return status;
}

int test_mem()
{
    return 0;
}

int main()
{
    test_fs();
    test_mem();

    printf("%s\n%d", __FILE__, __LINE__);

    return 0;
}

