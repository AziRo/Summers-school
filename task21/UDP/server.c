#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


int main()
{
    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in caddr, saddr;

    bzero(&saddr, sizeof(saddr));
    bzero(&saddr, sizeof(caddr));

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    saddr.sin_addr.s_addr = inet_addr("192.168.0.106");

    bind(fd, (struct sockaddr *) &saddr, sizeof(saddr));
    if (errno != 0) {
        printf("ERROR: %s\n",strerror(errno));
    }
    char buf[256];
    int size_dest = sizeof(caddr);

    recvfrom(fd, buf, 256, 0, (struct sockaddr *) &caddr, &size_dest);
    if (errno != 0) {
        printf("ERROR: %s\n",strerror(errno));
    }
    printf("%s\n", buf);
    strcpy(buf, "Hello client!");
    sendto(fd, buf, 256, 0, (struct sockaddr *) &caddr, size_dest);

    close(fd);

    return 0;
}
