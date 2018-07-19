#include "service.h"

int main()
{
    struct sockaddr_in saddr;

    bzero(&saddr, sizeof(saddr));

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5790);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.2");

    int size_dest;
    int request = REQ_POW;
    float result;

    struct pow_arg args;
    args.value = 15;
    args.power = 3;

    while(1){
        int pid = fork();
        if (pid == 0) {
            size_dest = sizeof(saddr);
            int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (fd < 0) {
                perror("Failed to create socket");
                exit(1);
            }

            if (connect(fd, (struct sockaddr *) &saddr, size_dest) < 0) {
                perror("Failed to connect to server");
                exit(1);
            }

            /* Отправление заявки */
            send(fd, &request, sizeof(request), 0);
            printf("Заявка отправлена.\n");

            /* Получение сигнала о принятии/отклонении заявки */
            recv(fd, &request, sizeof(int), 0);

            if(request == ACCEPTED) {
                printf("Заявка находится в состоянии обработки.\n");

                /* Отправление данных для выполнения заявки */
                send(fd, &args, sizeof(args), 0);

                /* Получение результата выполнения заявки */
                recv(fd, &result, sizeof(float), 0);
                printf("Результат выполнения заявки = %f\n", result);
            } else {
                perror("Заявка отклонена");
            }

            sleep(20);
            close(fd);
            exit(0);
        } else {
            sleep(1);
        }
    }

    return 0;
}
