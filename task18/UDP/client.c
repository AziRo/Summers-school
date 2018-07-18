#include "service.h"

int main()
{
    struct sockaddr_in saddr;

    bzero(&saddr, sizeof(saddr));

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5690);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int size_dest;
    int request = REQ_FACT;
    float result;

    struct pow_arg args;
    args.value = 15;
    args.power = 3;

    while(1){
        int pid = fork();
        if (pid == 0) {
            size_dest = sizeof(saddr);
            int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

            /* Отправление заявки */
            sendto(fd, &request, sizeof(request), 0, (struct sockaddr *) &saddr, size_dest);
            printf("Заявка отправлена.\n");

            /* Получение сигнала о принятии/отклонении заявки */
            recvfrom(fd, &request, sizeof(int), 0, (struct sockaddr *) &saddr, &size_dest);

            if(request == ACCEPTED) {
                printf("Заявка находится в состоянии обработки.\n");

                /* Отправление данных для выполнения заявки */
                sendto(fd, &args, sizeof(args), 0, (struct sockaddr *) &saddr, size_dest);

                /* Получение результата выполнения заявки */
                recvfrom(fd, &result, sizeof(float), 0, (struct sockaddr *) &saddr, &size_dest);
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
