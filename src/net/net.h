#ifndef NET_H
#define NET_H

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "fs.h"
#include "serialize.h"

#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)

// #define LISTEN_PORT "8080"
#define RES_LEN      10000
#define RES_BODY_LEN 1

// In bytes, not bits
#define REQ_TYPE_O 0
#define REQ_TYPE_L 1
#define REQ_FSID_O 1
#define REQ_FSID_L FSID_LEN
#define REQ_LEN (REQ_TYPE_L+REQ_FSID_L)
 
// Add headers eventually
// struct tfs_header {
//     int i;
// };

enum req_type {
    REQ_GET_FS,          /* get a filesystem by fsid */
    REQ_GET_FILE,        /* get a singular file from fs given fsid */
    REQ_PUT_FILE,        /* put a file into fs given fsid */
    REQ_NEW_FS,          /* make a new fs */
    REQ_GET_ALL_FSIDS,   /* get all fsids in the fsdb */
    REQ_MAX = 255
};

struct tfs_req {
    // struct tfs_header header;
    enum req_type type;
    uint8_t fsid[FSID_LEN];
    uint8_t filename;
};

enum res_type {
    RES_ERROR,     /* an error,  data=error message */
    RES_FILE,      /* a success, data=a file */
    RES_FS,        /* a success, data=a filesystem */
    RES_FSIDS,     /* a success, data=a list of fsids */
    RES_SUCCESS,   /* a general success, data=a general message */
    RES_MESG,      /* data=a general message */
    RES_MAX = 255
};

struct tfs_res { // TBI (TODO)
    // struct tfs_header header;
    enum res_type type;
    uint8_t data[RES_BODY_LEN];
};

/* protocol */
void pack_req(uint8_t **buf, struct tfs_req req);
void pack_res(uint8_t **buf, struct tfs_res res);
struct tfs_req unpack_req(uint8_t *req);
struct tfs_res unpack_res(uint8_t *res);
void print_req(struct tfs_req r);
void print_res(struct tfs_res r);

/* server */
int init_server(char *port);

/* client */
SOCKET init_client(char *ip, char *port);
struct fs *client_get_fs(SOCKET server, uint8_t tfsid[]);
int send_fs();
int send_file();

#endif
