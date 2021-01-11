#include "net/net.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("usage: client <port>\n");
        return 1;
    }

    SOCKET server;
    server = init_client("127.0.0.1", argv[1]);
    if (server == -1) {
        fprintf(stderr, "invalid server socket\n");
        return 1;
    }

    recv_fs(server);

    return status;
}
