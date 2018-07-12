#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>


#define MAX_MSG 256
#define MAX_NAME 64
#define MAX_USERS 64

#define TYPE_CONNECT 1L
#define TYPE_MESSAGE 2L

int user_count;

char usernames[MAX_USERS][MAX_NAME];

int msgid_out, msgid_in;


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


void *connect_handler(void *arg)
{
    int *msgid = (int*)arg;
    
    struct connectToServer msg;
    
    int msg_size = sizeof(struct message) - sizeof(long);
    
    /* Обработка подключения новых пользователей */
    while (1) {
     
        msgrcv(*msgid, &msg, msg_size, TYPE_CONNECT, 0);
        printf("User [%s] connected!\n", msg.name);
        strcpy(usernames[user_count], msg.name);
        ++user_count;
    
    }
    
}


void *msg_handler(void *arg)
{
    int *msgid = (int*)arg;
    
    struct message msg;
    
    int msg_size = sizeof(struct message) - sizeof(long);
    
    while (1) {
        
        /* Ожидание сообщения от пользователя */
        msgrcv(msgid_in, &msg, msg_size, TYPE_MESSAGE, 0);
         
        printf("User [%s] sent a message.\n", msg.from); 
          
        /* Рассылка полученного сообщения по всем пользователям */
        for (int i = 0; i < user_count; ++i) {
            strcpy(msg.to, usernames[i]);
            msgsnd(*msgid, &msg, msg_size, 0);
        }
        
    }
    
}


int main()
{
    /* Инициализация двух списков очередей */
    key_t key_out, key_in;
    
    
    key_in = ftok("./server.out", 'A');
    key_out = ftok("./server.out", 'B');
    
    msgid_out = msgget(key_out, IPC_CREAT | 0666);  //msg snd
    msgid_in = msgget(key_in, IPC_CREAT | 0666);    //msg rcv
    
    if (msgid_out == -1 || msgid_in == -1) {
        printf("Error msgget\n");
        exit(0);
    } else {
        printf("Success!\n");
    }
    
    
    pthread_t tid_connect, tid_msg;
    
    /* Создание потоков обработки подключения и получения сообщений */
    pthread_create(&tid_connect, NULL, &connect_handler, &msgid_in);
    pthread_create(&tid_msg, NULL, &msg_handler, &msgid_out);
    
    /* Ожидание завершения работы потоков */
    pthread_join(tid_connect, NULL);
    pthread_join(tid_msg, NULL);   
    
    return 0;
}
