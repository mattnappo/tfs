#include "net/net.h"

int test_get_fs()
{
    SOCKET server;
    server = init_client(argv[1], argv[2]);
    if (server == -1) {
        fprintf(stderr, "invalid server socket\n");
        return 1;
    }

    // Construct the fsid
    uint8_t tfsid[FSID_LEN] = { 0xab, 0xab, 0x12, 0x32,
                                0x46, 0xf6, 0xd3, 0xd4,
                                0x16, 0x2c, 0xff, 0xaf,
                                0xcd, 0xf3, 0xee, 0xe2 };
    // memset(tfsid, 0x00, FSID_LEN);

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

int test_get_file()
{
    SOCKET server;
    server = init_client(argv[1], argv[2]);
    if (server == -1) {
        fprintf(stderr, "invalid server socket\n");
        return 1;
    }

    // Construct the fsid from which the file will be fetched
    uint8_t tfsid[FSID_LEN] = { 0xab, 0xab, 0x12, 0x32,
                                0x46, 0xf6, 0xd3, 0xd4,
                                0x16, 0x2c, 0xff, 0xaf,
                                0xcd, 0xf3, 0xee, 0xe2 };
    char *filename = "files/testfile.txt"; // The file to be fetched
    // memset(tfsid, 0x00, FSID_LEN);

    struct file f = client_get_file(server, tfsid, filename);
    if (fs == NULL) {
        printf("client: received FS is null\n");
        return 0;
    }
    print_file(f, ASCII);

    destroy_fs(fs);
    destroy_file(f);
}

}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("usage: client <ip> <port>\n");
        return 1;
    }

    test_get_fs();

    return 0;
}
