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
#define N 5

int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in caddr, saddr;

    bzero(&saddr, sizeof(saddr));
    bzero(&saddr, sizeof(caddr));

    saddr.sin_family = AF_INET;
    saddr.sin_port = 5690;
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(fd, (struct sockaddr *) &saddr, sizeof(saddr));
    listen(fd, N);

    char buf[BUF_SIZE];
    int size_dest;

    while (1) {
        /* Принимаем новые подключения */
        size_dest = sizeof(caddr);
        int new_fd = accept(fd, (struct sockaddr *) &caddr, &size_dest);
        printf("New connection.\n");

        /* Создаём процесс обработки клиента */
        int pid = fork();
        if (pid == 0) {

            strcpy(buf, "You'r connected!");
            send(new_fd, buf, BUF_SIZE, 0);

            recv(new_fd, buf, BUF_SIZE, 0);
            printf("%s\n", buf);

            sprintf(buf, "Server %i: Hello client!", getpid());
            send(new_fd, buf, BUF_SIZE, 0);

            sleep(20);
            close(new_fd);
            exit(0);
        }
    }
    close(fd);

    return 0;
}
