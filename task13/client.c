#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>


#define MAX_MSG 256
#define MAX_NAME 64

#define TYPE_CONNECT 1L
#define TYPE_DISCONNECT 2L
#define TYPE_MESSAGE 3L
#define TYPE_SERVER 4L

char *name;
int quit = 0;

struct message {
    long type;
    char from[MAX_NAME];
    char to[MAX_NAME];
    char msg[MAX_MSG];
};


struct connectToServer {
    long type;
    char name[MAX_NAME];
};


char *_getline(int *size, int max_size)
{
    char *line = malloc(max_size * sizeof(char));
    
    int i;
    
    for (i = 0; i < max_size - 1; ++i) {
        
        read(STDOUT_FILENO, &line[i], 1);
        
        if (line[i] == '\n') {
            break;
        }
        
    }
    
    line[i] = 0;
    
    if (size != NULL)
        *size = i;
        
    return line;
}


void *rcv_handler(void *arg)
{
    int *msgid = (int*)arg; 
    
    struct message msg;
    
    int msg_size = sizeof(msg) - sizeof(long);
    
    /* Обработка приходящих сообщений */
    while (1) {
        
        msgrcv(*msgid, &msg, msg_size, TYPE_SERVER, 0);
        
        if (quit) break;
        
        printf("[%s]: %s\n", msg.from, msg.msg);
        
        usleep(200);
    }
    
}


void *snd_handler(void *arg)
{
    int *msgid = (int*)arg;
    char *mess;
    struct message msg;
    struct connectToServer msg_discon;
    
    strcpy(msg.from, name);
    strcpy(msg_discon.name, name);
    
    msg.type = TYPE_MESSAGE;
    msg_discon.type = TYPE_DISCONNECT;
    
    int msg_size = sizeof(msg) - sizeof(long);
    int msg_dis_size = sizeof(msg_discon) - sizeof(long);
    
    /* Отправление сообщения */
    while (1) {
        mess = _getline(NULL, MAX_MSG);
	    /* Отключение от сервера */
	    if (!strcmp(mess, "quit")) {
	        msgsnd(*msgid, &msg_discon, msg_dis_size, 0);
            quit = 1;
            free(mess);
            break;
	    }   
          
	    strcpy(msg.msg, mess); 
        
        msgsnd(*msgid, &msg, msg_size, 0);        
        
        
        free(mess);
    }
    
}


int main()
{
    key_t key_out, key_in;
    int msgid_out, msgid_in;
    
    key_out = ftok("./server.out", 'I');
    key_in = ftok("./server.out", 'F');
    
    msgid_out = msgget(key_out, 0);
    msgid_in = msgget(key_in, 0);
    
    if (msgid_out == -1 || msgid_in == -1) {
        printf("Error msgget\n");
        exit(0);
    } else {
        printf("Success!\n");
    }
    
    /* Регистрация на сервере */
    struct connectToServer msg_connect;
    
    int msg_size;
    
    write(1, "Write username: ", strlen("Write username: "));
    name = _getline(NULL, MAX_NAME);
    msg_connect.type = TYPE_CONNECT;
    strcpy(msg_connect.name, name);
    
    msg_size = sizeof(msg_connect) - sizeof(long);
    
    msgsnd(msgid_out, &msg_connect, msg_size, 0);
    
    pthread_t tid_snd, tid_rcv;
    
    /* Создание потоков отправления сообщений и получения сообщений */
    pthread_create(&tid_snd, NULL, &snd_handler, &msgid_out);
    pthread_create(&tid_rcv, NULL, &rcv_handler, &msgid_in);
    
    /* Ожидание завершения работы потоков */
    pthread_join(tid_snd, NULL);
    pthread_join(tid_rcv, NULL);
    
    
    free(name);
    
    return 0;
}



