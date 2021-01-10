#include "net.h"

int test_server()
{
    init_server();
    return 0;
}

int main()
{
    int status;
    status = test_server();

    return status;
}
