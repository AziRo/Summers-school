#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>


#define MSG_SIZE 256


int main()
{
    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    setsockopt(fd, SOL_SOCKET,SO_BROADCAST, &val, sizeof(val));

    struct sockaddr_in raddr, saddr;

    bzero(&raddr, sizeof(raddr));
    bzero(&saddr, sizeof(saddr));

    raddr.sin_family = AF_INET;
    raddr.sin_port = htons(7777);
    raddr.sin_addr.s_addr = inet_addr("192.168.0.255");

    bind(fd, (struct sockaddr *) &raddr, sizeof(raddr));

    char buf[MSG_SIZE];

    int size_dest = sizeof(saddr);

    recvfrom(fd, buf, MSG_SIZE, 0, (struct sockaddr *) &saddr, &size_dest);
    printf("%s\n", buf);

    close(fd);

    return 0;
}
