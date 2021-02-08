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

struct fs *client_get_fs(SOCKET server, uint8_t tfsid[])
{
    // Construct request
    struct tfs_req req = { .type = REQ_GET_FS, .body_len = 0 };
    memcpy(req.fsid, tfsid, FSID_LEN);
    uint8_t *packed;
    size_t packed_req_size = pack_req(&packed, req);

    // Send request
    int bytes_sent = send(server, packed, packed_req_size, 0);
    if (bytes_sent != packed_req_size) {
        fprintf(stderr, "send failed: did not send all req bytes\n");
        print_req(req, 1);
        free(packed);
        return NULL;
    }
    free(packed);

    // Read response
    uint8_t *raw_res = calloc(MAX_RES_LEN, 1);
    int recv_reslen = recv(server, raw_res, MAX_RES_LEN, 0);
    printf("received %d bytes.\n", recv_reslen);
    struct tfs_res res = unpack_res(raw_res);
    print_res(res, 0);

    if (res.type == RES_ERROR) {
        printf("%s\n", (char *) res.body);
        free(packed);
        free(raw_res);
        return NULL;
    }
    if (res.type != RES_FS) {
        printf("unimplemented response code for REQ_GET_FS: %d\n", (int)res.type);
        return NULL;
    }
    struct fs *dfs = deserialize_fs(res.body, res.body_len);
    free(raw_res);
    return dfs;
}
