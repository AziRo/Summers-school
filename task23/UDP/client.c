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
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <net/if.h>


#define MSG_SIZE 256
#define S_PORT 8888
#define D_PORT 1555

#define IPv4 0x800
#define CRC16 0x8005


struct udp_msg {
    struct ethhdr eth_hdr;
    struct iphdr ip_hdr;
    struct udphdr udp_hdr;
    char buff[MSG_SIZE];
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

    return ~(crc);
}


int main()
{
    int fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    int val = 1;
    if (errno != 0) {
        printf("ERROR1: %s\n",strerror(errno));
    }

    struct sockaddr_ll saddr;
    bzero(&saddr, sizeof(saddr));

    saddr.sll_family = AF_PACKET;
    saddr.sll_halen = ETH_ALEN;
    saddr.sll_ifindex = if_nametoindex("enp4s0f1");

    struct udp_msg msg;
    bzero(&msg, sizeof(msg));

    strcpy(msg.buff, "Hello server!");

    /* Заполнение UDP заголовка */
    msg.udp_hdr.source = htons(S_PORT);
    msg.udp_hdr.dest = htons(D_PORT);
    msg.udp_hdr.len = htons(MSG_SIZE + 8);
    msg.udp_hdr.check = gen_crc16((uint16_t *)&msg, sizeof(msg)/2);

    /* Заполнение IP заголовка */
    msg.ip_hdr.ihl = 5;
    msg.ip_hdr.version = 4;
    msg.ip_hdr.tos = 0;
    msg.ip_hdr.tot_len = htons(28 + MSG_SIZE);
    msg.ip_hdr.id = htonl(rand() % 65000);
    msg.ip_hdr.frag_off = 0;
    msg.ip_hdr.ttl = 64;
    msg.ip_hdr.protocol = IPPROTO_UDP;
    msg.ip_hdr.saddr = inet_addr("192.168.0.60");
    msg.ip_hdr.daddr = inet_addr("192.168.0.30");
    msg.ip_hdr.check = gen_crc16((uint16_t*) &(msg.ip_hdr), sizeof(msg.ip_hdr)/2);

    /* Заполнение заголовка канального уровня */
    msg.eth_hdr.h_source[0] = 0x54;
    msg.eth_hdr.h_source[1] = 0xab;
    msg.eth_hdr.h_source[2] = 0x3a;
    msg.eth_hdr.h_source[3] = 0x96;
    msg.eth_hdr.h_source[4] = 0x4f;
    msg.eth_hdr.h_source[5] = 0x32;

    msg.eth_hdr.h_dest[0] = 0xb8;
    msg.eth_hdr.h_dest[1] = 0x27;
    msg.eth_hdr.h_dest[2] = 0xeb;
    msg.eth_hdr.h_dest[3] = 0x20;
    msg.eth_hdr.h_dest[4] = 0xe4;
    msg.eth_hdr.h_dest[5] = 0x2c;

    msg.eth_hdr.h_proto = htons(IPv4);

    int size_dest = sizeof(saddr);

    sendto(fd, &msg, sizeof(msg), 0, (struct sockaddr *) &saddr, size_dest);

    if (errno != 0) {
        printf("ERROR2: %s\n",strerror(errno));
    }

    while(1) {
    	recvfrom(fd, &msg, sizeof(msg), 0, (struct sockaddr *) &saddr, &size_dest);
        if (msg.udp_hdr.dest == htons(S_PORT)) {
            printf("%s\n", msg.buff);
            break;
        }
    }
    close(fd);

    return 0;
}
