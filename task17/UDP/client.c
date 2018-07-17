#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>


#define BUF_SIZE 256


int main()
{
    struct sockaddr_in saddr;

    bzero(&saddr, sizeof(saddr));

    saddr.sin_family = AF_INET;
    saddr.sin_port = 5690;
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int size_dest;
    char buf[BUF_SIZE];

    while(1){
        int pid = fork();
        if (pid == 0) {
            size_dest = sizeof(saddr);
            int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

            /* Подключение к основному серверу */
            strcpy(buf, "New connection.");
            sendto(fd, buf, BUF_SIZE, 0, (struct sockaddr *) &saddr, size_dest);

            /* Получение данных о новом сервере */
            recvfrom(fd, buf, BUF_SIZE, 0, (struct sockaddr *) &saddr, &size_dest);
            printf("%s\n", buf);

            /* Обмен сообщениями с новым сервером */
            sprintf(buf, "Client %i: Hello server!", getpid());
            sendto(fd, buf, BUF_SIZE, 0, (struct sockaddr *) &saddr, size_dest);

            recvfrom(fd, buf, BUF_SIZE, 0, (struct sockaddr *) &saddr, &size_dest);
            printf("%s\n", buf);

            sleep(20);
            close(fd);
            exit(0);
        } else {
            sleep(1);
        }
    }

    return 0;
}
