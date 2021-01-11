#include "net.h"

// static uint8_t *pack_req_header(struct tfs_req header)
// {
// 
// }

size_t pack_req(uint8_t **buf, struct tfs_req req)
{
    int t = (int) req.type;
    *buf = calloc(sizeof(struct tfs_req), 1);
    memcpy(buf[0], (uint8_t *) &t, REQ_TYPE_L);
    // memcpy(&buf[REQ_FSID_O],
    //     req.fsid, REQ_FSID_L);

    printf("size: %lu\n", sizeof(struct tfs_req));
    printf("calculated size: %d\n", REQ_TYPE_L + REQ_FSID_L);
    return sizeof(struct tfs_req);

}
