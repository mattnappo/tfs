#include "net/net.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("usage: server <port>\n");
        return 1;
    }

    int status;
    status = init_server(argv[1]);

    return status;
}
