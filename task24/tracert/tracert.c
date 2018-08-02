#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>


#define MSG_SIZE 256

#define IPv4 0x800
#define CRC16 0x8005

#define D_PORT 39764
#define S_PORT 9999


struct icmphdr {
    uint8_t type;
    uint8_t code;
    uint16_t check;
} __attribute__((packed));


struct ping {
    struct iphdr ip_hdr;
    struct icmphdr icmp_hdr;
    uint16_t id;
    uint16_t num;
    uint32_t data;
};


struct tracert {
    struct icmphdr icmp;
    uint16_t len;
    struct iphdr ip_hdr;
    struct udphdr udp;
    uint32_t data;
} __attribute__((packed));


struct icmp_msg {
    struct iphdr ip_hdr;
    struct tracert icmp_hdr;
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


void *ping(void *arg)
{
    char *dest_addr = (char *)arg;

    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    int val = 1;
    setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &val, sizeof(val));
    if (errno != 0) {
        printf("ERROR_01: %s\n",strerror(errno));
    }

    struct sockaddr_in saddr;
    bzero(&saddr, sizeof(saddr));

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(D_PORT);
    saddr.sin_addr.s_addr = inet_addr(dest_addr);

    struct ping msg;
    bzero(&msg, sizeof(msg));

    /* Заполнение IP заголовка */
    msg.ip_hdr.ihl = 5;
    msg.ip_hdr.version = 4;
    msg.ip_hdr.tos = 0;
    msg.ip_hdr.tot_len = sizeof(msg);
    msg.ip_hdr.id = htonl(rand() % 65000);
    msg.ip_hdr.frag_off = 0;
    msg.ip_hdr.protocol = IPPROTO_ICMP;
    msg.ip_hdr.check = 0;
    msg.ip_hdr.saddr = inet_addr("192.168.0.106");
    msg.ip_hdr.daddr = inet_addr(dest_addr);

    /* Заполнение ICMP заголовка */
    msg.icmp_hdr.type = 8;
    msg.icmp_hdr.code = 0;
    msg.id = htons(rand() % 65000);

    int size_dest = sizeof(saddr);
    for (int i = 0; i < 255; ++i) {

        /* Заполнение ICMP заголовка */
        msg.num = htons(i + 1);
        msg.icmp_hdr.check = 0;
        msg.icmp_hdr.check = gen_crc16((uint16_t *)&(msg.icmp_hdr),
                             (sizeof(msg.icmp_hdr) + sizeof(uint64_t))/2);

        /* Изменение TTL */
        msg.ip_hdr.ttl = htons(i+1) >> 8;

        /* Посылка  ICMP пакета*/
        sendto(fd, &msg, sizeof(msg), 0, (struct sockaddr *) &saddr, size_dest);
        usleep(500);
    }

    if (errno != 0) {
        printf("ERROR_02: %s\n",strerror(errno));
    }
}


int main(int argc, char* argv[])
{
    if (argc == 1) {
        printf("No ip address specified\n");
        exit(0);
    }

    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    /* Ограничение времени ожидания при приёме */
    struct timeval val;
    val.tv_sec = 1;
    val.tv_usec = 500;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &val, sizeof(val));

    /* Создание потока передачи ICMP пакетов */
    pthread_t tid;
    pthread_create(&tid, 0, ping, (void *)argv[argc-1]);

    if (errno != 0) {
        printf("ERROR1: %s\n",strerror(errno));
        exit(0);
    }

    struct sockaddr_in saddr, addr;
    bzero(&saddr, sizeof(saddr));
    bzero(&addr, sizeof(addr));

    saddr.sin_family = AF_INET;
    saddr.sin_port = 80;
    saddr.sin_addr.s_addr = inet_addr(argv[argc-1]);

    struct icmp_msg msg;
    bzero(&msg, sizeof(msg));

    int size_dest = sizeof(saddr);
    int counter = 0;

    /* Приём ответов на ICMP пакеты */
    while(1) {

        recvfrom(fd, &msg, sizeof(msg), 0, (struct sockaddr *) &saddr, &size_dest);

        if (errno != 0) {
            printf("ERROR2: %s\n",strerror(errno));
            exit(0);
        }

        if(msg.icmp_hdr.icmp.type == 11 || msg.icmp_hdr.icmp.type == 0) {

            ++counter;

            addr.sin_addr.s_addr = msg.ip_hdr.saddr;
            char str[64];
            inet_ntop(AF_INET, &(addr.sin_addr), str, 32);
            printf("from %s ttl=%i\n", str, htons(msg.ip_hdr.ttl) >> 8);

            if(msg.icmp_hdr.icmp.type == 0) {
                pthread_cancel(tid);
                break;
            }
        }

        if(counter == 255 || msg.icmp_hdr.icmp.type == 3) {
            printf("Destination unreachable\n");
            break;
        }

    }
    pthread_join(tid, NULL);

    close(fd);

    return 0;
}
