#include "net.h"

void print_req(struct tfs_req r)
{
    printf("\nreq:\n  type: %d\n  fsid: 0x", r.type);
    for (int c = 0; c < FSID_LEN; c++)
        printf("%02x", r.fsid[c]);
    printf("\n\n");
}

void pack_req(uint8_t **buf, struct tfs_req req)
{
    *buf = calloc(REQ_LEN, 1);
    int t = (int) req.type;
    memcpy(buf[REQ_TYPE_O], (uint8_t *) &t, REQ_TYPE_L);
    memcpy(*buf+REQ_FSID_O, &req.fsid, REQ_FSID_L);
}

struct tfs_req unpack_req(uint8_t *req)
{
    enum req_type type = req[REQ_TYPE_O];
    uint8_t *fsid = malloc(FSID_LEN);
    memcpy(fsid, req+1, FSID_LEN);
    struct tfs_req dreq = { .type = type };
    memcpy(dreq.fsid, fsid, FSID_LEN);
    free(fsid);
    return dreq;
}

