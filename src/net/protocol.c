#include "net.h"

const char *emesgs[] = {
    "server error: server fs is too large to send"
};

void print_req(struct tfs_req r)
{
    printf("\nreq:\n  type: %d\n  fsid: 0x", r.type);
    for (int c = 0; c < FSID_LEN; c++)
        printf("%02x", r.fsid[c]);
    printf("\n\n");
}

size_t pack_req(uint8_t **buf, struct tfs_req req)
{
    *buf = calloc(REQ_LEN, 1);
    int t = (int) req.type;
    memcpy(buf[REQ_TYPE_OFF], (uint8_t *) &t, REQ_TYPE_LEN); // copy type into 0
    memcpy(*buf+REQ_FSID_OFF, &req.fsid, REQ_FSID_LEN); // copy fsid into [1,17]
    memcpy(*buf+REQ_BODYLEN_OFF, (uint8_t *) &req.body_len, 2); // copy body_len into [18,19]
    if (req.body_len >= REQ_BODY_LEN) {
        printf("body is too large to pack request.\n");
        return NULL;
    }
    if (req.body_len > 0) {
        memcpy(*buf+REQ_BODY_OFF, res.body, res.body_len);
    }
    return REQ_TYPE_LEN+FSID_LEN+UINT16_LEN
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
    printf("\nres:\n  type: %d\n  body_len: %u\n",
        r.type, r.body_len);
    if (show_body) {
        printf("  body: \n0x");
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
    memcpy(body, res+RES_BODY_OFF, RES_BODY_LEN);
    struct tfs_res dres = { .type = type, .body_len = body_len };
    memcpy(dres.body, body, body_len);
    free(body);
    return dres;
}

int send_err(SOCKET client, enum net_err err)
{
    uint8_t *body = (uint8_t *) emesgs[err];
    uint16_t body_len = strlen(emesgs[err]);
    printf("error message len: %d\n", body_len);
    struct tfs_res res = { .type = RES_ERROR, .body_len = body_len };
    memcpy(res.body, body, body_len);
    uint8_t *packed;
    size_t reslen = pack_res(&packed, res);
    int bytes_sent = send(client, (const void *) packed, reslen, 0);
    printf("sending error (%d bytes)\n", bytes_sent);
    return bytes_sent;
}
