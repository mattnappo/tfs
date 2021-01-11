#include "net/net.h"

int test_res()
{
    // This is the process for making a request
    uint8_t tfsid[FSID_LEN];
    // memset(tfsid, 0x41, 16);
    // for (int c = 0; c < 16; c++)
    //     printf("%x ", tfsid[c]);

    // struct tfs_req req = { .type = TFS_GET_FS, .fsid = {tfsid} };
    struct tfs_req req = { .type = TFS_GET_FS };
    memcpy(req.fsid, tfsid, FSID_LEN);

    uint8_t *packed;
    size_t packed_len = pack_req(&packed, req);
    printf("packed req:\n");
    for (int c = 0; c < packed_len; c++)
        printf("%x ", packed[c]);
    printf("\n");

    return 0;
}

int test_req()
{

    return 0;
}

int main()
{
    int status;
    status = test_res();
    printf("status: %d\n", status);

    // status = test_req();
    // printf("status: %d\n", status);

    return status;
}
