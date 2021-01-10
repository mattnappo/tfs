#include "net.h"

int test_server(char *port)
{
    init_server(port);
    return 0;
}

int main(int argc, char *argv[])
{
    int status;
    status = test_server(argv[1]);

    return status;
}
