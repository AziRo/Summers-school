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

#define SENDING_DELAY 500000


static int exit = 0;


static void sig_handler(int sig)
{
    printf("\n\t > SIGINT was received. Stopping process...\n");
    exit = 1;
}


int main()
{
    struct sockaddr_in raddr;
    struct in_addr local_addr;
    char buf[MSG_SIZE];
    int size_dest = sizeof(raddr);
    char loopch = 1;
    int it = 0;

    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (fd < 0)
    {
        perror("opening datagram socket");
        return 1;
    }

    signal(SIGINT, &sig_handler);

    bzero(&local_addr, sizeof(local_addr));
    bzero(&raddr, sizeof(raddr));

    raddr.sin_family = AF_INET;
    raddr.sin_port = htons(DEF_PORT);
    raddr.sin_addr.s_addr = inet_addr(DEF_MC_ADDR);

    if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loopch, sizeof(loopch)) < 0)
    {
        perror("setting IP_MULTICAST_LOOP:");
        close(fd);
        return 2;
    }

    local_addr.s_addr = inet_addr(DEF_LOCAL_ADDR);
    if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&local_addr, sizeof(local_addr)) < 0)
    {
        perror("setting local interface");
        return 3;
    }

    while(!exit)
    {
        sprintf(buf, "You have received a multicast message number %d!", ++it);
        if (sendto(fd, buf, MSG_SIZE, 0, (struct sockaddr *) &raddr, size_dest) < 0)
        {
            perror("sending datagram message");
        }

        usleep(SENDING_DELAY);
    }

    close(fd);

    return 0;
}
