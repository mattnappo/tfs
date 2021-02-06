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
    memcpy(buf[REQ_TYPE_OFF], (uint8_t *) &t, REQ_TYPE_LEN);
    memcpy(*buf+REQ_FSID_OFF, &req.fsid, REQ_FSID_LEN);
}

struct tfs_req unpack_req(uint8_t *req)
{
    enum req_type type = req[REQ_TYPE_OFF];
    uint8_t *fsid = malloc(FSID_LEN);
    memcpy(fsid, req+1, FSID_LEN);
    struct tfs_req dreq = { .type = type };
    memcpy(dreq.fsid, fsid, FSID_LEN);
    free(fsid);
    return dreq;
}

void print_res(struct tfs_res r, int show_body)
{
    printf("\nres:\n  type: %d\n  body_len: %u", r.type, r.body_len);
    if (show_body) {
        for (int b = 0; b < r.body_len; b++)
            printf("%02x", r.body[b]);
        printf("\n");
    }
    printf("\n");
}

size_t pack_res(uint8_t **buf, struct tfs_res res)
{
    if (res.body_len >= RES_BODY_LEN) {
        printf("error unpacking request: body len too long.");
        return 0;
    }
    *buf = calloc(RES_LEN, 1);
    int t = (int) res.type;
    memcpy(buf[RES_TYPE_OFF], (uint8_t *) &t, RES_TYPE_LEN);
    memcpy(*buf+1, (uint8_t *) &res.body_len, 2);
    memcpy(*buf+RES_BODY_OFF, res.body, res.body_len);
    return 1+2 + res.body_len;
}

struct tfs_res unpack_res(uint8_t *res)
{
    enum res_type type = res[RES_TYPE_OFF];
    uint16_t body_len = ((uint16_t) res[2] << 8) | res[1]; // little endian
    uint8_t *body = calloc(RES_BODY_LEN, 1);
    memcpy(body, res+1, RES_BODY_LEN);
    struct tfs_res dres = { .type = type, .body_len = body_len };
    memcpy(dres.body, body, body_len);
    free(body);
    return dres;
}

