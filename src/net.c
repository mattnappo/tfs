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
    fs_add_file(tfs, tfile3, 200);
    fs_add_file(tfs, tfile3, 600);
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

    // Read request
    uint8_t *request = calloc(REQ_SIZE, 1);
    int reqlen = recv(client_sd, request, REQ_SIZE, 0);
    if (reqlen <= 0) {
        fprintf(stderr, "recv failed: %d\n", reqlen);
    }
    printf("received %d bytes.\n", reqlen);
    printf("%.*s", reqlen, request);

    /* process the request */
    free(request);
    /* ----- */

    // Serve (will go in handler func)
    struct lbuffer temp_fs = get_temp_fs();
    uint8_t *tfs_buf = temp_fs.buf;
    size_t tfs_len = temp_fs.len;

    const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Connection: close\r\n"
        "Content-Type: text/plain\r\n\r\n"
        "fs bytes: ";
    int reslen = send(client_sd, response, strlen(response), 0); // strlen warning
    printf("sent %d of %d bytes.\n", reslen, (int) strlen(response));

    for (int i = 0; i < tfs_len; i++) {
        char b[4];
        sprintf(b, "%X ", tfs_buf[i]);
        if (send(client_sd, b, 1, 0) != 1) {
            fprintf(stderr, "failed to write all fs bytes");
            break;
        }
        //printf("%X ", tfs_buf[i]);
    }
    printf("wrote all %ld fs bytes\n", tfs_len);

    // reslen = send(client_sd, bytes, strlen(), 0); // strlen warning
    // printf("sent %d of %d bytes.\n", reslen, (int) 10000);
    

    // Cleanup
    CLOSESOCKET(client_sd);
    CLOSESOCKET(sd);
    free(tfs_buf);

    return 0;
}
