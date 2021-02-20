#include "net/net.h"

uint8_t tfsid[FSID_LEN] = {0xbc,0x21,0xab,0x12,0x4b,0xa6,0xc0,0xeb,0xd0,0x3a,0xad,0xb7,0x19,0x90,0xba,0x2a};

static struct fs *get_temp_fs()
{
    struct fs *tfs = new_fs();
    struct file tfile1 = new_file("files/testfile.txt");
    fs_add_file(tfs, tfile1, 0);
    destroy_file(tfile1);
    return tfs;
}

int test_client_get_fs(char *argv[])
{
    SOCKET server;
    server = init_client(argv[1], argv[2]);
    if (server == -1) {
        fprintf(stderr, "invalid server socket\n");
        return 1;
    }

    struct fs *fs = client_get_fs(server, tfsid);
    if (fs == NULL) {
        printf("client: received FS is null\n");
        return 1;
    }
    fs_list_files(*fs);
    destroy_fs(fs);
    return 0;
}

int test_client_get_file(char *argv[])
{
    SOCKET server;
    server = init_client(argv[1], argv[2]);
    if (server == -1) {
        fprintf(stderr, "invalid server socket\n");
        return 1;
    }

    char *filename = "files/testfile.txt"; // The file to be fetched
    // memset(tfsid, 0x00, FSID_LEN);

    struct file f = client_get_file(server, tfsid, filename);
    if (f.s == 0) {
        printf("client: received file is null\n");
        return 1;
    }

    print_file(f, ASCII);
    destroy_file(f);
    return 0;
}

int test_client_put_file(char *argv[])
{
    SOCKET server;
    server = init_client(argv[1], argv[2]);
    if (server == -1) {
        fprintf(stderr, "invalid server socket\n");
        return 1;
    }
    struct file f = new_file("files/newtestfile");
    int status = client_put_file(server, tfsid, f, 30);
    if (status != 0) {
        printf("test: error putting file\n");
        destroy_file(f);
        return status;
    }
    destroy_file(f);
    return 0;
}

int test_client_put_fs(char *argv[])
{
    SOCKET server;
    server = init_client(argv[1], argv[2]);
    if (server == -1) {
        fprintf(stderr, "invalid server socket\n");
        return 1;
    }
    struct fs *tfs = get_temp_fs();
    int status = client_put_fs(server, tfs);
    if (status != 0) {
        printf("test: error inserting fs.\n");
        destroy_fs(tfs);
        return status;
    }
    destroy_fs(tfs);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("usage: client <ip> <port>\n");
        return 1;
    }

    printf("\nGETTING FS\n");
    test_client_get_fs(argv);

    //printf("\nGETTING FILE\n");
    //test_client_get_file(argv);

    //printf("\nPUTTING FILE\n");
    //test_client_put_file(argv);

    //printf("\nGETTING (NEW) FS\n");
    //test_client_get_fs(argv);

    //printf("\nPUTTING FS\n");
    //test_client_put_fs(argv);

    return 0;
}
