#include "net.h"

const char *emesgs[] = {
    "server error: server fs is too large to send",
    "server error: server unable to process request",
    "server error: request body too large",
    "server error: file with name not exist",
    "server error: internal fs error"
};

void print_req(struct tfs_req r, int show_body)
{
    printf("\nreq:\n  type: %d\n  fsid: 0x", r.type);
    for (int i = 0; i < FSID_LEN; i++)
        printf("%02x", r.fsid[i]);
    printf("\n  body_len: %u\n", r.body_len);

    if (show_body) {
        printf("  body:\n0x");
        for (int b = 0; b < r.body_len; b++)
            printf("%02x", r.body[b]);
    }
    printf("\n\n");
}

size_t pack_req(uint8_t **buf, struct tfs_req req)
{
    size_t req_len = REQ_TYPE_LEN+FSID_LEN+UINT16_LEN+req.body_len;
    *buf = calloc(req_len, 1);
    int t = (int) req.type;
    memcpy(buf[REQ_TYPE_OFF], (uint8_t *) &t, REQ_TYPE_LEN); // copy type into 0
    memcpy(*buf+REQ_FSID_OFF, &req.fsid, REQ_FSID_LEN); // copy fsid into [1,16]
    memcpy(*buf+REQ_BODYLEN_OFF, (uint8_t *) &req.body_len, UINT16_LEN); // copy body_len into [17,18]
    if (req.body_len >= MAX_REQ_BODY_LEN) {
        printf("body is too large to pack request.\n");
        return 0;
    }
    // copy body into [19,...]
    if (req.body_len > 0)
        memcpy(*buf+REQ_BODY_OFF, req.body, req.body_len);
    return req_len;
}

struct tfs_req unpack_req(uint8_t *req)
{
    enum req_type type = req[REQ_TYPE_OFF]; // extract type
    uint8_t *fsid = malloc(FSID_LEN);
    memcpy(fsid, req+REQ_FSID_OFF, FSID_LEN); // extract fsid
    uint16_t body_len = ((uint16_t) req[REQ_BODYLEN_OFF+1] << 8)
        | req[REQ_BODYLEN_OFF]; // little endian
    struct tfs_req dreq = { .type = type, .body_len = body_len };
    memcpy(dreq.fsid, fsid, FSID_LEN);
    if (body_len > 0)
        memcpy(dreq.body, req+REQ_BODY_OFF, body_len);
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
    if (res.body_len >= MAX_RES_BODY_LEN) {
        printf("error packing request: req body is too big");
        return 0;
    }
    size_t res_len = RES_TYPE_LEN+UINT16_LEN+res.body_len;
    *buf = calloc(res_len, 1);
    int t = (int) res.type;
    memcpy(buf[RES_TYPE_OFF], (uint8_t *) &t, RES_TYPE_LEN);
    memcpy(*buf+1, (uint8_t *) &res.body_len, UINT16_LEN);
    memcpy(*buf+RES_BODY_OFF, res.body, res.body_len);
    return res_len;
}

struct tfs_res unpack_res(uint8_t *res)
{
    enum res_type type = res[RES_TYPE_OFF]; // extract type
    uint16_t body_len = ((uint16_t) res[RES_BODYLEN_OFF+1] << 8)
        | res[RES_BODYLEN_OFF]; // little endian
    struct tfs_res dres = { .type = type, .body_len = body_len };
    memcpy(dres.body, res+RES_BODY_OFF, body_len);
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
    free(packed);
    return bytes_sent;
}
