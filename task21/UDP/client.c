#include <stdio.h>
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
    struct udphdr hdr;
    char buff[MSG_SIZE];
};


struct udp_msg_in {
    struct iphdr ip_hdr;
    struct udphdr hdr;
    char buff[MSG_SIZE];
} __attribute__((packed));


int main()
{
    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);

    if (errno != 0) {
        printf("ERROR: %s\n",strerror(errno));
    }
    struct sockaddr_in saddr;
    bzero(&saddr, sizeof(saddr));

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(D_PORT);
    saddr.sin_addr.s_addr = inet_addr("192.168.0.106");

    struct udp_msg msg;
    struct udp_msg_in msg_in;
    bzero(&msg, sizeof(msg));
    bzero(&msg_in, sizeof(msg_in));

    /* Заполнение UDP заголовка */
    msg.hdr.source = htons(S_PORT);
    msg.hdr.dest = htons(D_PORT);
    msg.hdr.len = htons(MSG_SIZE + 8);
    msg.hdr.check = 0;

    int size_dest = sizeof(saddr);

    strcpy(msg.buff, "Hello server!");
    sendto(fd, &msg, MSG_SIZE + 8, 0, (struct sockaddr *) &saddr, size_dest);

    if (errno != 0) {
        printf("ERROR: %s\n",strerror(errno));
    }

    for(int i = 0; i < 5; ++i) {
    	recvfrom(fd, &msg_in, MSG_SIZE + 28, 0, (struct sockaddr *) &saddr, &size_dest);
        if (msg_in.hdr.dest == htons(S_PORT)) {
            printf("%s\n", msg_in.buff);
            break;
        }
    }
    close(fd);

    return 0;
}
