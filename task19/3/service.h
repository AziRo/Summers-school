#ifndef __SERVICE__H
#define __SERVICE__H


#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <math.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <sys/epoll.h>


#define BUF_SIZE 256
#define LSN_SIZE 5
#define POOL_SIZE 3


#define START_S 0
#define ERROR_S 1
#define CHECK_S 2
#define HANDLE_S 3

#define REQ_POW 0
#define REQ_FACT 1

#define MSG_TYPE_REQ 1

#define ACCEPTED 1
#define UNACCEPTED 0


struct request {
    long type;
    int request;
    int fd;
    struct sockaddr_in caddr;
};


struct fsm {
    int fd;
    int msgid;
    int request;
    struct sockaddr_in caddr;
};

struct pow_arg {
    float value;
    float power;
};


int factorial(int);

int start_state(int, int *, struct sockaddr_in *);
int start_state_tcp(struct fsm *);

int error_state(int, struct sockaddr_in);
int error_state_tcp(struct fsm *);

int check_state(int);

int handle_state(int, struct sockaddr_in, int);
int handle_state_tcp(struct fsm *);


void *FSM(void *);
void *FSM_TCP(void *);


#endif // __SERVICE__H
