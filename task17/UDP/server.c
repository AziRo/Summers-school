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
    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in caddr, saddr;

    bzero(&saddr, sizeof(saddr));
    bzero(&saddr, sizeof(caddr));

    saddr.sin_family = AF_INET;
    saddr.sin_port = 5690;
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(fd, (struct sockaddr *) &saddr, sizeof(saddr));

    char buf[BUF_SIZE];
    int size_dest = sizeof(caddr);

    while (1) {
        /* Принимаем новые подключения */
        recvfrom(fd, buf, BUF_SIZE, 0, (struct sockaddr *) &caddr, &size_dest);
        printf("%s\n", buf);

        /* Создаём новый сокет */
        int new_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        /* Отправляем сообщение клиенту и данные о endpoint */
        strcpy(buf, "You'r connected!");
        sendto(new_fd, buf, BUF_SIZE, 0, (struct sockaddr *) &caddr, size_dest);

        /* Создаём процесс обработки клиента */
        int pid = fork();
        if (pid == 0) {

            size_dest = sizeof(caddr);
            recvfrom(new_fd, buf, BUF_SIZE, 0, (struct sockaddr *) &caddr, &size_dest);
            printf("%s\n", buf);

            sprintf(buf, "Server %i: Hello client!", getpid());
            sendto(new_fd, buf, BUF_SIZE, 0, (struct sockaddr *) &caddr, size_dest);

            sleep(20);
            close(new_fd);
            exit(0);
        }
    }
    close(fd);

    return 0;
}
