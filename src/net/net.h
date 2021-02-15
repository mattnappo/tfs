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
#define UINT16_LEN   2 /* for the uint16_t body_len */

#define RES_TYPE_OFF 0
#define RES_TYPE_LEN 1
#define RES_BODYLEN_OFF 1
#define RES_BODY_OFF 3
#define MAX_RES_BODY_LEN MAX_PACKED_FS_LEN
#define MAX_RES_LEN (RES_TYPE_LEN+UINT16_LEN+MAX_RES_BODY_LEN)
/*                   type         body_len   body       */

#define REQ_TYPE_OFF 0
#define REQ_TYPE_LEN 1
#define REQ_FSID_OFF 1
#define REQ_FSID_LEN FSID_LEN
#define REQ_BODYLEN_OFF 17 /* len is 2 */
#define REQ_BODY_OFF 19
#define MAX_REQ_BODY_LEN MAX_FILE_LEN
#define MAX_REQ_LEN (REQ_TYPE_LEN+REQ_FSID_LEN+UINT16_LEN+MAX_REQ_BODY_LEN)
/*                   type         fsid         body_len   body       */
#define MIN_REQ_LEN (REQ_TYPE_LEN+REQ_FSID_LEN+UINT16_LEN)

enum net_err {
    ERR_FS_OVERFLOW,
    ERR_REQUEST_FAIL,
    ERR_BODY_OVERFLOW,
    ERR_FILE_NOT_EXIST,
    ERR_FS_FAIL
};
int send_err(SOCKET client, enum net_err err);
 
enum req_type {
    REQ_GET_FS,          /* get fs by fsid,   body=null */
    REQ_GET_FILE,        /* get file,         body=filename */
    REQ_PUT_FILE,        /* put file into fs, body=file bytes */
    REQ_NEW_FS,          /* make a new fs,    body=null */
    REQ_GET_ALL_FSIDS,   /* get all fsids in fsdb, body=null*/
    REQ_MAX = 255
};

struct tfs_req {
    enum req_type type;
    uint8_t fsid[FSID_LEN];
    uint16_t body_len;
    uint8_t body[MAX_REQ_BODY_LEN];
};

enum res_type {
    RES_ERROR,     /* an error,  body=error message */
    RES_FILE,      /* a success, body=a file */
    RES_FS,        /* a success, body=a filesystem */
    RES_FSIDS,     /* a success, body=a list of fsids */
    RES_SUCCESS,   /* a general success, body=a general message */
    RES_MESG,      /* body=a general message */
    RES_NULL,      /* request wasn't even sent */
    RES_MAX = 255
};

struct tfs_res {
    enum res_type type;
    uint16_t body_len;
    uint8_t body[MAX_RES_BODY_LEN];
};

/* protocol */
size_t pack_req(uint8_t **buf, struct tfs_req req);
size_t pack_res(uint8_t **buf, struct tfs_res res);
struct tfs_req unpack_req(uint8_t *req);
struct tfs_res unpack_res(uint8_t *res);
void print_req(struct tfs_req r, int show_body);
void print_res(struct tfs_res r, int show_body);

/* server */
int start_server(char *port);
/* server internal */
static int send_success(SOCKET client);
static int handle_conn (server_db *sdb, SOCKET client);
static int handle_req  (server_db *sdb, SOCKET client, struct tfs_req r);
static int handle_req_get_fs  (server_db *sdb, SOCKET client, struct tfs_req r);
static int handle_req_get_file(server_db *sdb, SOCKET client, struct tfs_req r);
static int handle_req_put_file(server_db *sdb, SOCKET client, struct tfs_req r);

/* client */
SOCKET init_client(char *ip, char *port);
struct fs*  client_get_fs  (SOCKET server, uint8_t tfsid[]);
struct file client_get_file(SOCKET server, uint8_t tfsid[], char *filename);
int         client_put_file(SOCKET server, uint8_t tfsid[], struct file f, uint16_t offset);
/* client internal */
static struct tfs_res client_exchange(
    SOCKET server, struct tfs_req req, enum res_type target);

#endif
