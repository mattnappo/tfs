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
        fprintf(stderr, "connect failed: %d\n", GETSOCKETERRNO());
        freeaddrinfo(server_addr);
        return -1;
    }
    freeaddrinfo(server_addr);

    return server_sd;
}

struct fs *client_get_fs(SOCKET server)
{
    // First, make the request to get an fs
    uint8_t tfsid[FSID_LEN];
    memset(tfsid, 0x00, FSID_LEN);
    struct tfs_req req = { .type = TFS_GET_FS };
    memcpy(req.fsid, tfsid, FSID_LEN);
    uint8_t *packed;
    pack_req(&packed, req);

    // Then, send it
    int bytes_sent = send(server, packed, REQ_LEN, 0);
    if (bytes_sent != REQ_LEN) {
        fprintf(stderr, "send failed: did not send all req bytes\n");
        print_req(req);
        return NULL;
    }
    free(packed);

    // Finally, read from server and make fs
    uint8_t recv_fs_buffer[RES_LEN];
    int bytes_recv = recv(server, recv_fs_buffer, RES_LEN, 0);
    printf("recv %d bytes\n", bytes_recv);
    struct fs *dfs = deserialize_fs(recv_fs_buffer, bytes_recv); // Because its raw for now
    return dfs;
}
