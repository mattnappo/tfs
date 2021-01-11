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
#define REQ_SIZE 4096
#define RES_SIZE 100000

#define REQ_TYPE_O 0
#define REQ_TYPE_L 8
#define REQ_FSID_O 9
#define REQ_FSID_L FSID_LEN
 
// Add headers eventually
// struct tfs_header {
//     int i;
// };

enum req_type {
    TFS_GET_FS,
    TFS_GET_FILE,
    TFS_PUT_FILE,
    TFS_MAX = 255
};

struct tfs_req {
    // struct tfs_header header;
    enum req_type type;
    uint8_t fsid[FSID_LEN];
};

struct tfs_res {
    // struct tfs_header header;

};

/* protocol */
size_t pack_req(uint8_t **buf, struct tfs_req req);
size_t pack_res(uint8_t **buf, struct tfs_res res);
struct tfs_req unpack_req(uint8_t *req, size_t len);
struct tfs_res unpack_res(uint8_t *res, size_t len);

/* server */
int init_server(char *port);
int handle_conn(SOCKET client);

/* client */
SOCKET init_client(char *ip, char *port);
struct fs *recv_fs(SOCKET server);
int send_fs();
int send_file();

#endif
