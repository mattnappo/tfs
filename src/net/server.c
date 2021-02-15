#include "net.h"

/*
static struct lbuffer get_temp_fs()
{
    struct fs *tfs = new_fs();
    struct file tfile1 = new_file("files/testfile.txt");
    struct file tfile2 = new_file("files/testfile2.txt");
    struct file tfile3 = new_file("files/test_file");
    fs_add_file(tfs, tfile1, 0);
    fs_add_file(tfs, tfile2, 90);
    fs_add_file(tfs, tfile3, 200);
    uint8_t *buffer;
    unsigned slen = serialize_fs(&buffer, tfs);
    destroy_file(tfile1);
    destroy_file(tfile2);
    destroy_file(tfile3);
    destroy_fs(tfs);
    struct lbuffer lbuff = { .buf = buffer, .len = slen  };
    return lbuff;
}

static struct fs *get_temp_fs_defined()
{
    struct fs *tfs = new_fs();
    struct file tfile1 = new_file("files/testfile.txt");
    fs_add_file(tfs, tfile1, 0);
    destroy_file(tfile1);
    return tfs;
}
*/

int start_server(char *port)
{
    // Create server socket sd
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    struct addrinfo *bind_addr;
    // getaddrinfo(0, LISTEN_PORT, &hints, &bind_addr);
    getaddrinfo(0, port, &hints, &bind_addr);
    SOCKET sd;
    sd = socket(bind_addr->ai_family, bind_addr->ai_socktype,
        bind_addr->ai_protocol);
    if (!ISVALIDSOCKET(sd)) {
        fprintf(stderr, "socket failed: %d\n", GETSOCKETERRNO());
        return 1;
    }

    // Bind
    int status;
    status = bind(sd, bind_addr->ai_addr, bind_addr->ai_addrlen);
    if (status) {
        fprintf(stderr, "bind failed: %d\n", GETSOCKETERRNO());
        freeaddrinfo(bind_addr);
        return 1;
    }
    freeaddrinfo(bind_addr);

    // Listen
    int backlog = 10;
    while (1) {
        status = listen(sd, backlog);
        if (status < 0) {
            fprintf(stderr, "listen failed: %d\n", GETSOCKETERRNO());
            return 1;
        }

        // Accept client
        struct sockaddr_storage client_addr;
        socklen_t client_len = sizeof(client_addr);
        SOCKET client_sd = accept(sd,
            (struct sockaddr *) &client_addr, &client_len);
        if (!ISVALIDSOCKET(client_sd)) {
            fprintf(stderr, "accept failed: %d\n", GETSOCKETERRNO());
            return 1;
        }
        char addr_buffer[100];
        getnameinfo((struct sockaddr *) &client_addr,
            client_len, addr_buffer, sizeof(addr_buffer), 0, 0, NI_NUMERICHOST);
        printf("%s\n", addr_buffer);

        // Handle the connection
        status = handle_conn(client_sd);
        if (status != 0) {
            fprintf(stderr, "handle_conn failed: %d\n", status);
            return 1;
        }

        // Cleanup
        CLOSESOCKET(client_sd);
    }
    CLOSESOCKET(sd);

    return 0;
}

static int send_success(SOCKET client)
{
    struct tfs_res res = { .type = RES_SUCCESS };
    print_res(res, 1);
    uint8_t *packed;
    size_t reslen = pack_res(&packed, res);
    int sent_reslen = send(client, (const void *) packed, reslen, 0);
    printf("sent %d of %lu bytes.\n", sent_reslen, reslen);
    free(packed);
    return sent_reslen;
}

static int handle_req_get_fs(server_db *sdb, SOCKET client, struct tfs_req r)
{
    // Look up in fsdb
    struct fs *fs = sdb_get_fs(sdb, r.fsid);
    uint8_t *fs_buf;
    unsigned fs_len = serialize_fs(&fs_buf, fs);
    if (fs_len >= MAX_RES_BODY_LEN) {
        free(fs_buf);
        send_err(client, ERR_FS_OVERFLOW);
        return -1;
    }

    // Make the res
    struct tfs_res res = { .type = RES_FS, .body_len = fs_len};
    memcpy(res.body, fs_buf, fs_len);
    print_res(res, 0);

    // Serialize and send
    uint8_t *packed;
    size_t reslen = pack_res(&packed, res);
    int sent_reslen = send(client, (const void *) packed, reslen, 0);
    printf("sent %d of %lu bytes.\n", sent_reslen, reslen);
    free(fs_buf);
    free(packed);
    return 0;
}

static int handle_req_get_file(server_db *sdb, SOCKET client, struct tfs_req r)
{
    // Fetch the fs (eventually with fsid)
    struct fs *fs = get_temp_fs_defined();
    if (r.body_len >= FILENAME_SIZE) {
        send_err(client, ERR_BODY_OVERFLOW);
        destroy_fs(fs);
        return 1;
    }

    // Extract the filename, +1 to leave a null byte at the end
    uint8_t filename[FILENAME_SIZE];
    memcpy(filename, r.body, r.body_len+1);
    struct file f = fs_get_file(fs, (char *) filename);
    if (f.s <= 0) {
        send_err(client, ERR_FILE_NOT_EXIST);
        destroy_fs(fs);
        return 1;
    }

    // Make the res
    struct tfs_res res = { .type = RES_FILE, .body_len = f.s };
    memcpy(res.body, f.bytes, f.s);
    print_res(res, 1);

    // Pack
    uint8_t *packed;
    size_t reslen = pack_res(&packed, res);
    int sent_reslen = send(client, (const void *) packed, reslen, 0);
    printf("sent %d of %lu bytes.\n", sent_reslen, reslen);
    free(packed);

    destroy_file(f);
    destroy_fs(fs);
    return 0;
}

static int handle_req_put_file(server_db *sdb, SOCKET client, struct tfs_req r)
{
    // Fetch the fs
    struct fs *fs = get_temp_fs_defined();
    if (r.body_len >= MAX_FILE_LEN) {
        send_err(client, ERR_BODY_OVERFLOW);
        destroy_fs(fs);
        return 1;
    }
    
    // Extract the offset and reconstruct the file
    uint16_t offset = ((uint16_t) r.body[1] << 8) | r.body[0];
    struct file f = deserialize_file(r.body+2, r.body_len-2);
    // Insert the file into the fs
    // TODO: Call the calc_offset() or find_valid_offset() func here
    if (fs_add_file(fs, f, offset) != 0) {
        send_err(client, ERR_FS_FAIL);
        destroy_fs(fs);
        return 1;
    }
    send_success(client);
    destroy_fs(fs);
    return 0;
}

static int handle_req(server_db *sdb, SOCKET client, struct tfs_req r)
{
    switch (r.type) {
    case REQ_GET_FS:
        return handle_req_get_fs(sdb, client, r);

    case REQ_GET_FILE:
        return handle_req_get_file(sdb, client, r);

    case REQ_PUT_FILE:
        return handle_req_put_file(sdb, client, r);

    case REQ_NEW_FS:
        break;

    case REQ_GET_ALL_FSIDS:
        break;

    default:
        fprintf(stderr, "invalid request type option: REQ%d\n", r.type);
        return 1;
    }
    return 0;
}

static int handle_conn(server_db *sdb, SOCKET client)
{
    // Read request
    uint8_t *request = calloc(MAX_REQ_LEN, 1);
    size_t reqlen = recv(client, request, MAX_REQ_LEN, 0);
    if (reqlen < MIN_REQ_LEN || reqlen >= MAX_REQ_LEN) {
        fprintf(stderr, "recv failed, received %ld bytes (%d MIN, %d MAX).\n",
            reqlen, MIN_REQ_LEN, MAX_REQ_LEN);
        send_err(client, ERR_REQUEST_FAIL);
        free(request);
        return 1;
    }
    printf("received %ld bytes.\n", reqlen);

    // Process the request
    int status;
    struct tfs_req unpacked_req = unpack_req(request);
    print_req(unpacked_req, 1);

    status = handle_req(sdb, client, unpacked_req);
    if (status > 0) {
        send_err(client, ERR_REQUEST_FAIL);
        free(request);
        return 1;
    }
    free(request);

    return 0;
}

