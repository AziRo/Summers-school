#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h> 


#define MAX_MSG 256
#define MAX_NAME 64
#define MAX_USERS 64


char *buff;

int sem_mut_id,
    sem_user_count_id;

/* sem 1 - lock, sem 0 unlock */
struct sembuf   lock_connect[2] = {{0, 0, 0}, {0, 1, 0}}, 
                unlock_connect = {0, -1, 0},
                lock[2] = {{1, 0, 0}, {1, 1, 0}}, 
                unlock = {1, -1, 0},
                inc = {0, 1, 0};


void *connect_handler(void *arg)
{
    int user_count;
    char msg[MAX_MSG];
    while (1) {
        if (user_count < semctl(sem_user_count_id, 0, GETVAL)){
            user_count = semctl(sem_user_count_id, 0, GETVAL);
            sprintf(msg, "[Server]: Количество пользователей: %i", user_count);
            strcpy(buff, msg);
            semop(sem_mut_id, lock_connect, 1);
        }
        usleep(200);
    }
    
    
}


void *msg_handler(void *arg)
{
    
    
    
}


int main()
{
    int shmid;
    key_t key;
    
    key = ftok("./server.out", 'A');
    shmid = shmget(key, MAX_MSG * sizeof(char), IPC_CREAT | 0666);
    buff = shmat(shmid, NULL, 0);
    
    strcpy(buff, "Hello ShM!"); //del---------------------------
    
    /* Создание бинарного семафора */
    key = ftok("./Makefile", 'B');
    sem_mut_id = semget(key, 1, IPC_CREAT | 0666);
    
    /* Создание счётчика клиентов */
    key = ftok("./Makefile", 'A');
    sem_user_count_id = semget(key, 1, IPC_CREAT | 0666);
    
    semctl(sem_user_count_id, 0, SETVAL, 0);
    semctl(sem_mut_id, 0, SETVAL, 0);
    
    
    pthread_t tid_connect, tid_msg;
    
    /* Создание потоков обработки подключения и получения сообщений */
    pthread_create(&tid_connect, NULL, &connect_handler, NULL);
    pthread_create(&tid_msg, NULL, &msg_handler, NULL);
    
    /* Ожидание завершения работы потоков */
    pthread_join(tid_connect, NULL);
    pthread_join(tid_msg, NULL);
    
    shmdt(buff);   
    
    return 0;
}
