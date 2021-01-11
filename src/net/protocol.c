#include "net.h"

// static uint8_t *pack_req_header(struct tfs_req header)
// {
// 
// }

size_t pack_req(uint8_t **buf, struct tfs_req req)
{
    *buf = calloc(REQ_SIZE, 1);
    int t = (int) req.type;
    memcpy(buf[REQ_TYPE_O], (uint8_t *) &t, REQ_TYPE_L);
    memcpy(*buf+REQ_FSID_O, &req.fsid, REQ_FSID_L);
    return REQ_SIZE;
}


