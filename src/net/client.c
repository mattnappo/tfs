#include "net.h"

SOCKET init_client(char *ip, char *port)
{
    int status;

    // Build server address
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *server_addr;
    status = getaddrinfo(ip, port, &hints, &server_addr);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo failed: %d\n", GETSOCKETERRNO());
        return -1;
    }
    char address_buffer[100]; // Print addr
    char service_buffer[100];
    getnameinfo(server_addr->ai_addr, server_addr->ai_addrlen,
        address_buffer, sizeof(address_buffer),
        service_buffer, sizeof(service_buffer),
        NI_NUMERICHOST);
    printf("%s %s\n", address_buffer, service_buffer);

    // Connect
    SOCKET server_sd;
    server_sd = socket(server_addr->ai_family, server_addr->ai_socktype,
        server_addr->ai_protocol);
    if (!ISVALIDSOCKET(server_sd)) {
        fprintf(stderr, "socket failed: %d\n", GETSOCKETERRNO());
        return -1;
    }
    status = connect(server_sd,
        server_addr->ai_addr, server_addr->ai_addrlen);
    if (status != 0) {
        fprintf(stderr, "conncet failed: %d\n", GETSOCKETERRNO());
        return -1;
    }
    freeaddrinfo(server_addr);

    return server_sd;
}

struct fs *recv_fs(SOCKET server)
{
    return NULL;

}
