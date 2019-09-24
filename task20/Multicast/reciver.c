#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>


#define MSG_SIZE 256
#define DEF_PORT 7777
#define DEF_MC_ADDR "224.0.0.22"
#define DEF_LOCAL_ADDR "10.25.72.32"


static int exit = 0;


static void sig_handler(int sig)
{
    printf("\n\t > SIGINT was received. Stopping process...\n");
    exit = 1;
}


int main()
{
    struct sockaddr_in raddr, saddr;
    struct ip_mreq mreq;
    char buf[MSG_SIZE];
    int size_dest = sizeof(saddr);
    int reuse = 1, it = 0;

    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (fd < 0)
    {
        perror("opening datagram socket");
        return 1;
    }

    signal(SIGINT, &sig_handler);

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        perror("setting SO_REUSEADDR");
        close(fd);
        return 2;
    }

    bzero(&raddr, sizeof(raddr));
    bzero(&saddr, sizeof(saddr));
    bzero(&mreq, sizeof(mreq));

    raddr.sin_family = AF_INET;
    raddr.sin_port = htons(DEF_PORT);
    raddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(fd, (struct sockaddr *) &raddr, sizeof(raddr)) < 0)
    {
        perror("binding datagram socket");
        close(fd);
        return 3;
    }

    mreq.imr_multiaddr.s_addr = inet_addr(DEF_MC_ADDR);
    mreq.imr_interface.s_addr = inet_addr(DEF_LOCAL_ADDR);

    if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
    {
        perror("adding multicast group");
        close(fd);
        return 4;
    }

    while(!exit)
    {
        if (recvfrom(fd, buf, MSG_SIZE, 0, (struct sockaddr *) &saddr, (socklen_t *)&size_dest) < 0)
        {
            perror("receiving datagram message");
        }
        printf("%d) %s\n", ++it, buf);
    }

    if (setsockopt(fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
    {
        perror("drop multicast group");
    }

    close(fd);

    return 0;
}
