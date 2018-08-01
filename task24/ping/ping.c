#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netinet/ip.h>
#include <errno.h>


#define MSG_SIZE 256

#define IPv4 0x800
#define CRC16 0x8005

struct icmphdr {
    uint8_t type;
    uint8_t code;
    uint16_t check;
    uint16_t id;
    uint16_t num;
    uint8_t data;
} __attribute__((packed));


struct icmp_msg_rep {
    struct iphdr ip_hdr;
    struct icmphdr icmp_hdr;
} __attribute__((packed));

uint16_t gen_crc16(const uint16_t *data, uint16_t size)
{
    int i = 0;
    uint32_t crc = 0;

    while (i < size) {
        crc = crc + data[i];
        if (crc > 0xFFFF) {
            crc -= 0x10000;
            crc += 0x1;
        }

        ++i;
    }

    return ~crc;
}


int main(int argc, char* argv[])
{
    if (argc == 1) {
        printf("No ip address specified\n");
        exit(0);
    }

    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    int val = 1;
    if (errno != 0) {
        printf("ERROR1: %s\n",strerror(errno));
        exit(0);
    }

    struct sockaddr_in saddr;
    bzero(&saddr, sizeof(saddr));

    saddr.sin_family = AF_INET;
    saddr.sin_port = 80;
    saddr.sin_addr.s_addr = inet_addr(argv[argc-1]);

    struct icmphdr icmp;
    struct icmp_msg_rep msg_reply;
    bzero(&icmp, sizeof(icmp));
    bzero(&msg_reply, sizeof(msg_reply));

    /* Заполнение ICMP заголовка */
    icmp.type = 8;
    icmp.code = 0;
    icmp.id = htons(rand() % 65000);

    int size_dest = sizeof(saddr);

    for (int i = 0; i < 20; ++i) {
        /* Заполнение ICMP заголовка */
        icmp.num = htons(i + 1);
        icmp.check = 0;
        icmp.check = gen_crc16((uint16_t *)&(icmp), sizeof(icmp)/2);

        /* Echo (ping) request */
        sendto(fd, &icmp, sizeof(icmp), 0, (struct sockaddr *) &saddr, size_dest);

        if (errno != 0) {
            printf("ERROR2: %s\n",strerror(errno));
            exit(0);
        }

        /* Echo (ping) reply */
        while (1) {
            recvfrom(fd, &msg_reply, sizeof(msg_reply), 0, (struct sockaddr *) &saddr, &size_dest);

            if (msg_reply.ip_hdr.saddr == saddr.sin_addr.s_addr
                && msg_reply.icmp_hdr.type == 0) {

                printf("from %s icmp_seq=%i ttl=%i\n", argv[argc-1],
                        htons(msg_reply.icmp_hdr.num), msg_reply.ip_hdr.ttl);

                break;
            }
        }
    }
    close(fd);

    return 0;
}
