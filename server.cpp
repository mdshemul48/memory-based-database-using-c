#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <cassert>

#define PORT 5000

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

static void do_something(int connfd)
{
    char rbuf[64] = {};
    ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
    if (n < 0)
    {
        msg("read() error");
        return;
    }
    printf("client says: %s\n", rbuf);

    char wbuf[] = "world";
    if (write(connfd, wbuf, strlen(wbuf)) < 0)
    {
        die("can't write on sock");
    }
}

static int32_t read_full(int fd, char *buf, size_t n)
{
    while (n > 0)
    {
        ssize_t rv = read(fd, buf, n);
        if (rv <= 0)
        {
            return -1;
        }

        assert((size_t)rv <= n);

        n -= (size_t)rv;
        buf += rv;
    }
    return 0;
}

static int32_t write_all(int fd, char *buf, size_t n)
{
    while (n > 0)
    {
        ssize_t rv = write(fd, buf, n);
        if (rv <= 0)
        {
            return -1;
        }

        assert((size_t)rv <= n);
        n -= (size_t)rv;

        buf += rv;
    }

    return 0;
}

int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        die("Can't create socket.");
    }

    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(PORT);
    addr.sin_addr.s_addr = ntohl(0);

    if (bind(fd, (const sockaddr *)&addr, sizeof(addr)))
        die("can't bind port");

    if (listen(fd, SOMAXCONN))
        die("Can't listen");

    printf("Listening on %i\n", PORT);
    while (true)
    {
        struct sockaddr_in client_addr = {};
        socklen_t socklen = sizeof(client_addr);

        int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);

        if (connfd < 0)
        {
            continue;
        }

        do_something(connfd);
        close(connfd);
    }

    return 0;
}