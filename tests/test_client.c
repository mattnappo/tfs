#include "net/net.h"

static struct fs *get_temp_fs()
{
    struct fs *tfs = new_fs();
    struct file tfile1 = new_file("files/testfile.txt");
    fs_add_file(tfs, tfile1, 0); destroy_file(tfile1); return tfs;
}

int test_client_get_fs(char *argv[])
{
    SOCKET server;
    server = init_client(argv[1], argv[2]);
    if (server == -1) {
        fprintf(stderr, "invalid server socket\n");
        return 1;
    }

    // Construct the fsid
    uint8_t tfsid[FSID_LEN] = {0xaa,0x6c,0xf2,0x8a,0x73,0x2f,0x5c,0x9d,0xf6,0xc6,0x1f,0x5e,0xc3,0x54,0x51,0x1c};

    struct fs *fs = client_get_fs(server, tfsid);
    if (fs == NULL) {
        printf("client: received FS is null\n");
        return 0;
    }
    fs_list_files(*fs);
    struct file f = fs_get_file(fs, "files/test_file");
    print_file(f, ASCII);

    destroy_fs(fs);
    destroy_file(f);
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

    // Construct the fsid from which the file will be fetched
    uint8_t tfsid[FSID_LEN] = { 0x8d,0xc3,0xe5,0xdc,  
                                0x8a,0x31,0x52,0xd7,  
                                0x3f,0x7f,0x8b,0xb5,  
                                0x5b,0x95,0x13,0xb2 };
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
    uint8_t tfsid[FSID_LEN] = { 0x8d,0xc3,0xe5,0xdc,  
                                0x8a,0x31,0x52,0xd7,  
                                0x3f,0x7f,0x8b,0xb5,  
                                0x5b,0x95,0x13,0xb2 };
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
    //destroy_fs(tfs);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("usage: client <ip> <port>\n");
        return 1;
    }

    test_client_get_fs(argv);
    //test_client_get_file(argv);
    //test_client_put_file(argv);
    //test_client_put_fs(argv);

    return 0;
}
