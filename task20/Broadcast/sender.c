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
    setsockopt(fd, SOL_SOCKET,SO_BROADCAST, &val, sizeof(val));

    struct sockaddr_in raddr;

    bzero(&raddr, sizeof(raddr));

    raddr.sin_family = AF_INET;
    raddr.sin_port = htons(7777);
    raddr.sin_addr.s_addr = inet_addr("192.168.0.255");

    char buf[MSG_SIZE];
    int size_dest = sizeof(raddr);

    strcpy(buf, "You have received a broadcast message!");
    sendto(fd, buf, MSG_SIZE, 0, (struct sockaddr *) &raddr, size_dest);

    close(fd);

    return 0;
}
