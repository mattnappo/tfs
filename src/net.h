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

#define LISTEN_PORT "8080"
#define REQ_SIZE 100000
 
enum tfs_type {
    TFS_FS,   // An entire fs
    TFS_FILE, // One file
};

struct tfs_header {
    enum tfs_type data_type;
};

int init_server(char *port);
void send_fs();
void send_file();
void recv_fs();
void recv_file();

#endif
