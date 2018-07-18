#include "service.h"


int main()
{
    /* Создание основного сокета */
    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd < 0) {
        perror("Failed to create socket");
        exit(1);
    }

    struct sockaddr_in caddr, saddr;

    bzero(&saddr, sizeof(saddr));
    bzero(&saddr, sizeof(caddr));

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5690);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(fd, (struct sockaddr *) &saddr, sizeof(saddr));

    /* Создание очереди заявок */
    key_t key = ftok("./server.out", 'A');
    int msgid = msgget(key, IPC_CREAT | 0664);

    if(msgid < 0) {
        perror("Failed to create message queue");
        exit(1);
    }

    /* Создание пула потоков обработки заявок */
    pthread_t tid[POOL_SIZE];
    struct fsm data;

    data.msgid = msgid;

    for(int i = 0; i < POOL_SIZE; ++i) {
        /* Создание нового сокета */
        int new_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (new_fd < 0) {
            perror("Failed to create socket");
            exit(1);
        }

        data.fd = new_fd;

        pthread_create(&tid[i], NULL, &FSM, &data);
    }

    int request;
    int size_dest;

    struct request msg;
    msg.type = MSG_TYPE_REQ;
    int msg_size = sizeof(msg) - sizeof(long);

    while (1) {
        /* Принимаем новые заявки и отправляем их в очередь*/
        size_dest = sizeof(caddr);
        recvfrom(fd, &request, sizeof(int), 0, (struct sockaddr *) &caddr, &size_dest);
        printf("Request %i\n", request);

        msg.request = request;
        msg.caddr = caddr;
        msgsnd(msgid, &msg, msg_size, 0);
    }

    close(fd);

    /* Ожидание завершения работы потоков */
    for (int i = 0 ; i < POOL_SIZE; ++i) {
        pthread_join(tid[i], NULL);
    }
    return 0;
}


int factorial(int n)
{
    int fact = 1;

    for(int i = 2; i <= n; ++i)
    {
        fact *= i;
    }

    return fact;
}


int start_state(int msgid, int *request, struct sockaddr_in *caddr)
{
    printf("State: start.\n");
    struct request msg;
    int msg_size = sizeof(msg) - sizeof(long);

    /* Принятие заявки */
    msgrcv(msgid, &msg, msg_size, MSG_TYPE_REQ, 0);
    *request = msg.request;
    *caddr = msg.caddr;

    return CHECK_S;
}


int error_state(int fd, struct sockaddr_in caddr)
{
    int accepted = UNACCEPTED;

    /* Отправляем клиенту сообщение об отклонении его заявки */
    int size_dest = sizeof(caddr);
    sendto(fd, &accepted, sizeof(int), 0, (struct sockaddr *) &caddr, size_dest);

    perror("Invalid request");

    return START_S;
}


int check_state(int request)
{
    printf("State: check.\n");
    int state;
    /* Проверка заявки */
    switch (request) {
        case REQ_POW:
            state = HANDLE_S;
            break;
        case REQ_FACT:
            state = HANDLE_S;
            break;
        default:
            state = ERROR_S;
    }

    return state;
}


int handle_state(int fd, struct sockaddr_in caddr, int request)
{

    printf("State: handle.\n");

    int state, accepted = ACCEPTED;
    float result = 0;
    int size_dest;
    struct pow_arg args;

    /* Отправляем клиенту сообщение о принятии его заявки и данные о endpoint */
    size_dest = sizeof(caddr);
    sendto(fd, &accepted, sizeof(int), 0, (struct sockaddr *) &caddr, size_dest);

    /* Получение данных для выполнения заявки */
    size_dest = sizeof(caddr);
    recvfrom(fd, &args, sizeof(args), 0, (struct sockaddr *) &caddr, &size_dest);

    /* Выполнение заявки */
    switch (request) {
        case REQ_POW:
            result = powf(args.value, args.power);
            sendto(fd, &result, sizeof(float), 0, (struct sockaddr *) &caddr, size_dest);
            state = START_S;
            break;
        case REQ_FACT:
            result = factorial(args.value);
            sendto(fd, &result, sizeof(float), 0, (struct sockaddr *) &caddr, size_dest);
            state = START_S;
            break;
        default:
            state = ERROR_S;
    }

    printf("Handle request completed!\n");

    return state;
}


void *FSM(void *arg)
{
    struct fsm *data = (struct fsm *)arg;
    int state = START_S;

    while (1) {

        switch (state) {
            /* Ожидание заявки */
            case START_S:
                state = start_state(data->msgid, &(data->request), &(data->caddr));
                break;

            /* Обработка ошибок */
            case ERROR_S:
                state = error_state(data->fd, data->caddr);
                break;

            /* Проверка заявки */
            case CHECK_S:
                state = check_state(data->request);
                break;

            /* Выполнение заявки */
            case HANDLE_S:
                state = handle_state(data->fd, data->caddr, data->request);
                break;
            default:
                perror("Failed to FSM.");
                exit(1);

        }

    }
}
