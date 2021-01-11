#include "net.h"

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
    return dreq;
}

