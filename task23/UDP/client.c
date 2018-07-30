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
#define D_PORT 9999

#define IPv4 0x800
#define CRC16 0x8005


struct udp_msg {
    struct ethhdr eth_hdr;
    struct iphdr ip_hdr;
    struct udphdr udp_hdr;
    char buff[MSG_SIZE];
} __attribute__((packed));


uint16_t gen_crc16(const uint8_t *data, uint16_t size)
{
    uint16_t out = 0;
    int bits_read = 0, bit_flag;

    if (data == NULL)
        return 0;

    while (size > 0) {
        bit_flag = out >> 15;

        out <<= 1;
        out |= (*data >> bits_read) & 1;

        bits_read++;
        if (bits_read > 7) {
            bits_read = 0;
            data++;
            size--;
        }

        if (bit_flag)
            out ^= CRC16;
    }

    int i;
    for (i = 0; i < 16; ++i) {
        bit_flag = out >> 15;
        out <<= 1;
        if(bit_flag)
            out ^= CRC16;
    }

    uint16_t crc = 0;
    i = 0x8000;
    int j = 0x0001;
    for (; i != 0; i >>=1, j <<= 1) {
        if (i & out) crc |= j;
    }

    return crc;
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
    saddr.sll_ifindex = if_nametoindex("wlp3s0");

    struct udp_msg msg;
    bzero(&msg, sizeof(msg));

    /* Заполнение UDP заголовка */
    msg.udp_hdr.source = htons(S_PORT);
    msg.udp_hdr.dest = htons(D_PORT);
    msg.udp_hdr.len = htons(MSG_SIZE + 8);
    msg.udp_hdr.check = gen_crc16((const uint8_t *)&msg, sizeof(msg));


    /* Заполнение IP заголовка */
    msg.ip_hdr.ihl = 5;
    msg.ip_hdr.version = 4;
    msg.ip_hdr.tos = 0;
    msg.ip_hdr.tot_len = htons(28 + MSG_SIZE);
    msg.ip_hdr.id = htonl(rand() % 65000);
    msg.ip_hdr.frag_off = 0;
    msg.ip_hdr.ttl = 64;
    msg.ip_hdr.protocol = IPPROTO_UDP;
    msg.ip_hdr.check = gen_crc16((const uint8_t *)&(msg.ip_hdr), sizeof(msg.ip_hdr));
    msg.ip_hdr.saddr = inet_addr("127.0.0.1");
    msg.ip_hdr.daddr = inet_addr("192.168.0.106");


    /* Заполнение заголовка канального уровня */
    msg.eth_hdr.h_source[0] = 0xc8;
    msg.eth_hdr.h_source[1] = 0xff;
    msg.eth_hdr.h_source[2] = 0x28;
    msg.eth_hdr.h_source[3] = 0x40;
    msg.eth_hdr.h_source[4] = 0x2b;
    msg.eth_hdr.h_source[5] = 0xb9;

    msg.eth_hdr.h_dest[0] = 0xf8;
    msg.eth_hdr.h_dest[1] = 0x1a;
    msg.eth_hdr.h_dest[2] = 0x67;
    msg.eth_hdr.h_dest[3] = 0xc3;
    msg.eth_hdr.h_dest[4] = 0xe5;
    msg.eth_hdr.h_dest[5] = 0x16;

    msg.eth_hdr.h_proto = htons(IPv4);

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
            break;
        }
    }
    close(fd);

    return 0;
}
