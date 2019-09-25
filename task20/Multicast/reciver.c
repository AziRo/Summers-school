#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <stdlib.h>

#define MSG_SIZE 256
#define DEF_PORT 7777
#define DEF_MC_ADDR "235.2.2.22"
#define DEF_LOCAL_ADDR "10.25.72.32"

#define MAX_PORT 65535
#define MAX_ADDR_LEN 16


static uint16_t def_port = DEF_PORT;
static char     def_mc_addr[MAX_ADDR_LEN];
static char     def_local_addr[MAX_ADDR_LEN];

static int stop = 0;

static struct option long_options[] =
{
    {"help",    no_argument,        0, 'h'},
    {"port",    optional_argument,  0, 'p'},
    {"mip",     optional_argument,  0, 'm'},
    {"lip",     optional_argument,  0, 'l'},

    {0, 0, 0, 0}
};


static void print_help(void)
{
    printf("Usage:\n"
           "\t--port Set local port for bindings\n"
           "\t--mip  Set multicast ip\n"
           "\t--lip  Set local ip for adding to multicast group\n"
           "\t--help Show this message\n");
}


static int proc_opt(int argc, char *argv[])
{
    int c;

    def_port = DEF_PORT;
    strcpy(def_mc_addr, DEF_MC_ADDR);
    strcpy(def_local_addr, DEF_LOCAL_ADDR);

    while (1)
    {
        c = getopt_long (argc, argv, "", long_options, NULL);
        if (c == -1)
            break;

        switch (c)
        {
            case 'p':
            {
                if (!optarg) break;
                def_port = atoi(optarg);
                break;
            }

            case 'm':
            {
                if (!optarg) break;
                strncpy(def_mc_addr, optarg, sizeof(def_mc_addr));
                break;
            }

            case 'l':
            {
                if (!optarg) break;
                strncpy(def_local_addr, optarg, sizeof(def_local_addr));
                break;
            }

            case 'h':
            default:
            {
                print_help();
                return 1;
            }
        }
    }

    return 0;
}


static void sig_handler(int sig)
{
    printf("\n\t > SIGINT was received. Stopping process...\n");
    stop = 1;
}


int main(int argc, char **argv)
{
    struct sockaddr_in raddr, saddr;
    struct ip_mreq mreq;
    char buf[MSG_SIZE];
    int size_dest = sizeof(saddr);
    int reuse = 1, it = 0;
    struct timeval recv_timeout;

    if (proc_opt(argc, argv))
    {
        return -1;
    }

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
    raddr.sin_port = htons(def_port);
    raddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(fd, (struct sockaddr *) &raddr, sizeof(raddr)) < 0)
    {
        perror("binding datagram socket");
        close(fd);
        return 3;
    }

    mreq.imr_multiaddr.s_addr = inet_addr(def_mc_addr);
    mreq.imr_interface.s_addr = inet_addr(def_local_addr);

    if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
    {
        perror("adding multicast group");
        close(fd);
        return 4;
    }

    recv_timeout.tv_sec = 5;
    recv_timeout.tv_usec = 0;

    if (setsockopt (fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout)) < 0)
    {
        perror("setting timeout to receive");
        close(fd);
        return 5;
    }

    while(!stop)
    {
        if (recvfrom(fd, buf, MSG_SIZE, 0, (struct sockaddr *) &saddr, (socklen_t *)&size_dest) >= 0)
        {
            printf("%d) %s\n", ++it, buf);
        }
    }

    if (setsockopt(fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
    {
        perror("drop multicast group");
    }

    close(fd);

    return 0;
}
