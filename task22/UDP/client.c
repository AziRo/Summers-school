#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <errno.h>


#define MSG_SIZE 256
#define S_PORT 8888
#define D_PORT 9999


struct udp_msg {
    struct iphdr ip_hdr;
    struct udphdr udp_hdr;
    char buff[MSG_SIZE];
} __attribute__((packed));


int main()
{
    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    int val = 1;
    setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &val, sizeof(val));
    if (errno != 0) {
        printf("ERROR1: %s\n",strerror(errno));
    }

    struct sockaddr_in saddr;
    bzero(&saddr, sizeof(saddr));

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(D_PORT);
    saddr.sin_addr.s_addr = inet_addr("192.168.0.106");

    struct udp_msg msg;
    bzero(&msg, sizeof(msg));

    /* Заполнение UDP заголовка */
    msg.udp_hdr.source = htons(S_PORT);
    msg.udp_hdr.dest = htons(D_PORT);
    msg.udp_hdr.len = htons(MSG_SIZE + 8);
    msg.udp_hdr.check = 0;


    /* Заполнение IP заголовка */
    msg.ip_hdr.ihl = 5;
    msg.ip_hdr.version = 4;
    msg.ip_hdr.tos = 0;
    msg.ip_hdr.tot_len = 28 + MSG_SIZE;
    msg.ip_hdr.id = htonl(rand() % 65000);
    msg.ip_hdr.frag_off = 0;
    msg.ip_hdr.ttl = 2;
    msg.ip_hdr.protocol = IPPROTO_UDP;
    msg.ip_hdr.check = 0;
    msg.ip_hdr.saddr = inet_addr("127.0.0.1");
    msg.ip_hdr.daddr = inet_addr("192.168.0.106");

    int size_dest = sizeof(saddr);

    strcpy(msg.buff, "Hello server!");
    sendto(fd, &msg, sizeof(msg), 0, (struct sockaddr *) &saddr, size_dest);

    if (errno != 0) {
        printf("ERROR2: %s\n",strerror(errno));
    }

    for(int i = 0; i < 5; ++i) {
    	recvfrom(fd, &msg, MSG_SIZE + 28, 0, (struct sockaddr *) &saddr, &size_dest);
        if (msg.udp_hdr.dest == htons(S_PORT)) {
            printf("%s\n", msg.buff);
        }
    }
    close(fd);

    return 0;
}
