#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

static void msg(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
}

static void die(const char *msg)
{
    int err = errno;
    fprintf(stderr, "[%d] %s: %s\n", err, msg, strerror(err));
    abort();
}

int main()
{

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        die("can't connect to the socket.");
    }

    struct sockaddr_in addr = {};

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);
    addr.sin_port = ntohs(5000);

    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));

    if (rv)
        die("can't connect");

    char msg[] = "hello";
    write(fd, msg, strlen(msg));

    char rbuf[64] = {};
    ssize_t n = read(fd, rbuf, sizeof(rbuf) - 1);

    if (n < 0)
        die("can't read from sock");

    printf("Server says: %s\n", rbuf);
    close(fd);
    return 0;
}