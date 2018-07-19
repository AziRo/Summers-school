#include "service.h"

#define MAX(A, B) ({ A > B ? A : B; })


int main()
{
    /* Создание основного сокета UDP и TCP */
    int fd_udp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    int fd_tcp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd_udp < 0 || fd_tcp < 0) {
        perror("Failed to create socket");
        exit(1);
    }

    struct sockaddr_in caddr_udp, caddr_tcp, saddr_udp, saddr_tcp;

    bzero(&saddr_udp, sizeof(saddr_udp));
    bzero(&saddr_tcp, sizeof(saddr_tcp));
    bzero(&caddr_udp, sizeof(caddr_udp));
    bzero(&caddr_tcp, sizeof(caddr_tcp));

    saddr_udp.sin_family = AF_INET;
    saddr_udp.sin_port = htons(5690);
    saddr_udp.sin_addr.s_addr = inet_addr("127.0.0.1");

    saddr_tcp.sin_family = AF_INET;
    saddr_tcp.sin_port = htons(5790);
    saddr_tcp.sin_addr.s_addr = inet_addr("127.0.0.2");

    bind(fd_udp, (struct sockaddr *) &saddr_udp, sizeof(saddr_udp));
    bind(fd_tcp, (struct sockaddr *) &saddr_tcp, sizeof(saddr_tcp));

    listen(fd_tcp, LSN_SIZE);

    /* Создание очереди заявок для UDP и TCP */
    key_t key1 = ftok("./Makefile", 'A');
    int msgid_udp = msgget(key1, IPC_CREAT | 0664);

    key_t key2 = ftok("./server.out", 'B');
    int msgid_tcp = msgget(key2, IPC_CREAT | 0664);

    if(msgid_udp < 0 || msgid_tcp < 0) {
        perror("Failed to create message queue");
        exit(1);
    }

    /* Создание пула потоков обработки заявок */
    pthread_t tid_tcp[POOL_SIZE];
    pthread_t tid_udp[POOL_SIZE];
    struct fsm data_tcp, data_udp;

    /* Пул на UDP */
    data_udp.msgid = msgid_udp;

    for(int i = 0; i < POOL_SIZE; ++i) {
        /* Создание нового сокета */
        int new_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (new_fd < 0) {
            perror("Failed to create socket");
            exit(1);
        }

        data_udp.fd = new_fd;

        pthread_create(&tid_udp[i], NULL, &FSM, &data_udp);
    }

    /* Пул на TCP */
    data_tcp.msgid = msgid_tcp;

    for(int i = 0; i < POOL_SIZE; ++i) {
        pthread_create(&tid_tcp[i], NULL, &FSM_TCP, &data_tcp);
    }

    int request;
    int size_dest_tcp, size_dest_udp;

    fd_set read_set;

    struct request msg_udp, msg_tcp;
    msg_udp.type = MSG_TYPE_REQ;
    msg_tcp.type = MSG_TYPE_REQ;
    int msg_size_udp = sizeof(msg_udp) - sizeof(long);
    int msg_size_tcp = sizeof(msg_tcp) - sizeof(long);

    int nfds = MAX(fd_tcp, fd_udp) + 1;

    /* Приём заявок от клиентов */
    while (1) {

        FD_ZERO(&read_set);
        FD_SET(fd_udp, &read_set);
        FD_SET(fd_tcp, &read_set);

        if (select(nfds, &read_set, NULL, NULL, NULL) < 0) {
            perror("Failed to select events");
            exit(1);
        }

        /* Принимаем новые заявки и отправляем их в очередь */
        if (FD_ISSET(fd_udp, &read_set)) {

            size_dest_udp = sizeof(caddr_udp);
            recvfrom(fd_udp, &request, sizeof(int), 0, (struct sockaddr *) &caddr_udp, &size_dest_udp);
            printf("UDP: Request %i\n", request);

            msg_udp.request = request;
            msg_udp.caddr = caddr_udp;
            msgsnd(msgid_udp, &msg_udp, msg_size_udp, 0);

        }
        if(FD_ISSET(fd_tcp, &read_set)) {

            size_dest_tcp = sizeof(caddr_tcp);
            int new_fd = accept(fd_tcp, (struct sockaddr *) &caddr_tcp, &size_dest_tcp);
            recv(new_fd, &request, sizeof(int), 0);
            printf("TCP: Request %i\n", request);

            msg_tcp.request = request;
            msg_tcp.caddr = caddr_tcp;
            msg_tcp.fd = new_fd;
            msgsnd(msgid_tcp, &msg_tcp, msg_size_tcp, 0);

        }

    }

    close(fd_tcp);
    close(fd_udp);


    /* Ожидание завершения работы потоков */
    for (int i = 0 ; i < POOL_SIZE; ++i) {
        pthread_join(tid_udp[i], NULL);
    }

    for (int i = 0 ; i < POOL_SIZE; ++i) {
        pthread_join(tid_tcp[i], NULL);
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

//------------------------------------UDP------------------------------------//

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

//------------------------------------TCP------------------------------------//

int start_state_tcp(struct fsm *data)
{
    printf("State: start.\n");
    struct request msg;
    int msg_size = sizeof(msg) - sizeof(long);

    /* Принятие заявки */
    msgrcv(data->msgid, &msg, msg_size, MSG_TYPE_REQ, 0);
    data->request = msg.request;
    data->caddr = msg.caddr;
    data->fd = msg.fd;

    return CHECK_S;
}


int error_state_tcp(struct fsm *data)
{
    int accepted = UNACCEPTED;

    /* Отправляем клиенту сообщение об отклонении его заявки */
    send(data->fd, &accepted, sizeof(int), 0);

    perror("Invalid request");

    return START_S;
}


int handle_state_tcp(struct fsm *data)
{

    printf("State: handle.\n");

    int state, accepted = ACCEPTED;
    float result = 0;
    struct pow_arg args;

    /* Отправляем клиенту сообщение о принятии его заявки и данные о endpoint */
    send(data->fd, &accepted, sizeof(int), 0);

    /* Получение данных для выполнения заявки */
    recv(data->fd, &args, sizeof(args), 0);

    /* Выполнение заявки */
    switch (data->request) {
        case REQ_POW:
            result = powf(args.value, args.power);
            send(data->fd, &result, sizeof(float), 0);
            state = START_S;
            break;
        case REQ_FACT:
            result = factorial(args.value);
            send(data->fd, &result, sizeof(float), 0);
            state = START_S;
            break;
        default:
            state = ERROR_S;
    }

    printf("Handle request completed!\n");

    return state;
}


void *FSM_TCP(void *arg)
{
    struct fsm *data = (struct fsm *)arg;
    int state = START_S;

    while (1) {

        switch (state) {
            /* Ожидание заявки */
            case START_S:
                state = start_state_tcp(data);
                break;

            /* Обработка ошибок */
            case ERROR_S:
                state = error_state_tcp(data);
                break;

            /* Проверка заявки */
            case CHECK_S:
                state = check_state(data->request);
                break;

            /* Выполнение заявки */
            case HANDLE_S:
                state = handle_state_tcp(data);
                break;
            default:
                perror("Failed to FSM_TCP.");
                exit(1);

        }

    }
}
