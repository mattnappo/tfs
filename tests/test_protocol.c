#include "net/net.h"

int test_res()
{
    // This is the process for making a request

    // Make the request
    uint8_t tfsid[FSID_LEN];
    memset(tfsid, 0x41, 16);
    printf("tfsid before: ");
    for (int c = 0; c < 16; c++)
        printf("%x ", tfsid[c]);
    printf("\n");

    struct tfs_req req = { .type = TFS_PUT_FILE };
    memcpy(req.fsid, tfsid, FSID_LEN);

    printf("tfsid inside: ");
    for (int c = 0; c < 16; c++)
        printf("%x ", req.fsid[c]);
    printf("\n\n\n");

    // Pack it
    uint8_t *packed;
    size_t packed_len = pack_req(&packed, req);
    printf("packed req: 0x");
    for (int i = 0; i < packed_len; i++)
        printf("%02x ", packed[i]);
    printf("\n\n");

    free(packed);
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
