#include "net.h"

struct lbuffer {
    uint8_t *buf;
    size_t len;
};

static struct lbuffer get_temp_fs()
{
    struct fs *tfs = new_fs();
    struct file tfile1 = new_file("testfile.txt");
    struct file tfile2 = new_file("testfile2.txt");
    struct file tfile3 = new_file("Makefile");
    fs_add_file(tfs, tfile1, 0);
    fs_add_file(tfs, tfile2, 50);
    fs_add_file(tfs, tfile3, 100);
    uint8_t *buffer;
    unsigned slen = serialize_fs(&buffer, tfs);
    destroy_file(tfile1);
    destroy_file(tfile2);
    destroy_file(tfile3);
    struct lbuffer lbuff = { .buf = buffer, .len = slen  };
    return lbuff;
}

int init_server(char *port)
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
    status = listen(sd, 10);
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

    status = handle_conn(client_sd);
    if (status != 0) {
        fprintf(stderr, "handle_conn failed: %d\n", status);
        return 1;
    }

    // Cleanup
    CLOSESOCKET(client_sd);
    CLOSESOCKET(sd);

    return 0;
}

int handle_conn(SOCKET client)
{
    // Read request
    uint8_t *request = calloc(REQ_SIZE, 1);
    int reqlen = recv(client, request, REQ_SIZE, 0);
    if (reqlen <= 0) {
        fprintf(stderr, "recv failed: %d\n", reqlen);
    }
    printf("received %d bytes.\n", reqlen);
    printf("%.*s", reqlen, request);

    /* process the request */
    free(request);
    /* ------------------- */

    struct lbuffer temp_fs = get_temp_fs();
    uint8_t *tfs_buf = temp_fs.buf;
    size_t tfs_len = temp_fs.len;

    int reslen = send(client, (const void *)tfs_buf, tfs_len, 0);
    printf("sent %d of %lu bytes.\n", reslen, tfs_len);

    free(tfs_buf);

    return 0;
}
