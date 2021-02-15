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

// Pack and send a request, return the server's response
static struct tfs_res client_exchange(
    SOCKET server, struct tfs_req req, enum res_type target)
{
    printf("--- client making request ---\n");
    print_req(req, 1);
    printf("-----------------------------\n");
    uint8_t *packed;
    size_t packed_req_size = pack_req(&packed, req);
    printf("\n begin raw req \n");
    for (int b=0;b<packed_req_size;b++) {
        printf("%02x ", packed[b]);
    }
    printf("\n end raw req \n");

    // Send request
    int bytes_sent = send(server, packed, packed_req_size, 0);
    if (bytes_sent != packed_req_size) {
        fprintf(stderr, "send failed: did not send all req bytes\n");
        free(packed);
        struct tfs_res eres = { .type = RES_NULL };
        return eres;
    }
    free(packed);

    // Read response
    uint8_t *raw_res = calloc(MAX_RES_LEN, 1);
    int recv_reslen = recv(server, raw_res, MAX_RES_LEN, 0);
    printf("received %d bytes.\n", recv_reslen);
    struct tfs_res res = unpack_res(raw_res);
    printf("--- server response ---\n");
    print_res(res, 1);
    printf("-----------------------\n");

    // Check for errors
    if (res.type == RES_ERROR) {
        printf("%s\n", (char *) res.body);
        free(raw_res);
        struct tfs_res eres = { .type = RES_NULL };
        return eres;
    }
    if (res.type != target) {
        printf("unimplemented response code for REQ%d: RES%d\n",
            (int)target, (int)res.type);
        free(raw_res);
        struct tfs_res eres = { .type = RES_NULL };
        return eres;
    }
    free(raw_res);
    return res;
}

struct fs *client_get_fs(SOCKET server, uint8_t tfsid[])
{
    // Construct request
    struct tfs_req req = { .type = REQ_GET_FS, .body_len = 0 };
    memcpy(req.fsid, tfsid, FSID_LEN);
    struct tfs_res res = client_exchange(server, req, RES_FS);
    if (res.type == RES_NULL) {
        printf("unable to exchange with server\n");
        return NULL;
    }

    struct fs *dfs = deserialize_fs(res.body, res.body_len);
    return dfs;
}

struct file client_get_file(SOCKET server, uint8_t tfsid[], char *filename)
{
    // Make req
    size_t filename_len = strlen(filename);
    if (filename_len >= FILENAME_SIZE) {
        printf("filename '%ld' too large\n", filename_len);
        return (struct file) {};
    }
    struct tfs_req req = {
        .type = REQ_GET_FILE,
        .body_len = filename_len+1 // +1 for the null term
    };
    memcpy(req.body, filename, filename_len);
    memcpy(req.fsid, tfsid, FSID_LEN);

    // Exchange
    struct tfs_res res = client_exchange(server, req, RES_FILE);
    if (res.type == RES_NULL) {
        printf("unable to exchange with server\n");
        return (struct file) {};
    }

    // Reconstruct file
    struct file file = { .s = res.body_len };
    memcpy(file.name, filename, filename_len);
    file.bytes = malloc(res.body_len);
    memcpy(file.bytes, res.body, res.body_len);
    print_file(file, ASCII);
    return file;
}

int client_put_file(
    SOCKET server, uint8_t tfsid[], struct file f, uint16_t offset)
{
    // Make req
    if (strlen(f.name) >= FILENAME_SIZE) {
        printf("filename too long\n");
        return 1;
    }
    if (f.s >= MAX_FILE_LEN) {
        printf("file too big\n");
        return 1;
    }
    uint8_t *buf;
    unsigned slen = serialize_file(&buf, f);
    if (slen+2 >= MAX_REQ_BODY_LEN) {
        printf("serialized file is too large to send\n");
        free(buf);
        return 1;
    }
    struct tfs_req req = { .type = REQ_PUT_FILE, .body_len = 2+slen };
    memcpy(req.fsid, tfsid, FSID_LEN);
    // First 2 bytes are for the file offset, provided by client
    // The rest of the body is the serialized file
    memcpy(req.body, (uint8_t *) &offset, 2);
    memcpy(req.body+2, buf, slen);
    free(buf);

    // Get res
    struct tfs_res res = client_exchange(server, req, RES_SUCCESS);
    if (res.type == RES_NULL) {
        printf("unable to exchange with server\n");
        return 1;
    }
    printf("successfully put file\n");
    return 0;
}
