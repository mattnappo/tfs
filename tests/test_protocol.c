#include "net/net.h"

int test_req()
{
    // Make fsid
    uint8_t tfsid[FSID_LEN];
    memset(tfsid, 0x41, 16);
    printf("tfsid before: ");
    for (int c = 0; c < 16; c++)
        printf("%x ", tfsid[c]);
    printf("\n");

    // Make body
    size_t body_len = 30;
    uint8_t *body = malloc(body_len);
    memset(body, 0x07, body_len);

    // Make res
    struct tfs_req req = { .type = REQ_PUT_FILE, .body_len = body_len };
    memcpy(req.fsid, tfsid, FSID_LEN); // copy fsid
    memcpy(req.body, body, body_len); // copy body
    free(body);

    // Pack it
    uint8_t *packed;
    size_t req_size = pack_req(&packed, req);
    printf("packed req: 0x");
    for (int i = 0; i < req_size; i++)
        printf("%02x ", packed[i]);
    printf("\n\nreq_size: %lu\n", req_size);

    // Unpack it
    struct tfs_req unpacked = unpack_req(packed);
    print_req(unpacked, 1);
    for (int i = 0; i < unpacked.body_len; i++)
        printf("%02x ", unpacked.body[i]);
    printf("\n");

    free(packed);
    return 0;
}

int test_res()
{
    // Make the res
    uint8_t body[100];
    memset(body, 0x41, 100);
    struct tfs_res res = { .type = RES_MESG, .body_len = 100 };
    memcpy(res.body, body, 100);
    print_res(res, 0);

    // Serialize
    uint8_t *packed;
    size_t packed_len = pack_res(&packed, res);
    for (int i = 0; i < packed_len; i++)
        printf("%02x ", packed[i]);
    printf("\n\n");

    // Deserialize
    struct tfs_res unpacked = unpack_res(packed);
    print_res(unpacked, 1);

    free(packed);
    return 0;
}

int main()
{
    int status;
    status = test_req();
    printf("status: %d\n", status);

    status = test_res();
    printf("status: %d\n", status);

    return status;
}
