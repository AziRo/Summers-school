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
#define TYPE_DISCONNECT 2L
#define TYPE_MESSAGE 3L
#define TYPE_SERVER 4L

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
    struct message msg_out;
    
    msg_out.type = TYPE_SERVER;
    strcpy(msg_out.from, "SERVER");
    
    int msg_size = sizeof(msg) - sizeof(long);
    int msg_o_size = sizeof(msg_out) - sizeof(long);
    
    char buff[256];
    
    /* Обработка подключения новых пользователей и отключения старых */
    while (1) {
     
        msgrcv(*msgid, &msg, msg_size, -TYPE_DISCONNECT, 0);
        
        if (msg.type == TYPE_DISCONNECT) {
            
            sprintf(buff, "User [%s] disconnected.", msg.name);
            printf("%s\n", buff);
            strcpy(msg_out.msg, buff);
        
        } else {
            sprintf(buff, "User [%s] connected!", msg.name);
            printf("%s\n", buff);
            strcpy(msg_out.msg, buff);
                
            strcpy(usernames[user_count], msg.name);
            ++user_count;
        }
        
        for(int i = 0; i < user_count; ++i)
            msgsnd(msgid_out, &msg_out, msg_o_size, 0);
        
        if(msg.type == TYPE_DISCONNECT)
            --user_count;
    }
    
}


void *msg_handler(void *arg)
{
    int *msgid = (int*)arg;
    
    struct message msg;
    
    int msg_size = sizeof(msg) - sizeof(long);
    
    while (1) {
        
        /* Ожидание сообщения от пользователя */
        msgrcv(msgid_in, &msg, msg_size, TYPE_MESSAGE, 0);
         
        printf("User [%s] sent a message.\n", msg.from); 
        msg.type = TYPE_SERVER;  
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
    
    
    key_in = ftok("./server.out", 'I');
    key_out = ftok("./server.out", 'F');
    
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
