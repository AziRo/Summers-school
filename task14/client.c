#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>


#define MAX_MSG 256
#define MAX_NAME 64

char *buff;
char *name;

int sem_mut_id,
    sem_user_count_id;

/* sem 1 - lock, sem 0 unlock */
struct sembuf   lock_connect[2] = {{0, 0, 0}, {0, 1, 0}}, 
                unlock_connect = {0, -1, 0},
                lock[2] = {{1, 0, 0}, {1, 1, 0}}, 
                unlock = {1, -1, 0},
                inc = {0, 1, 0};

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
    while (1) {
        //semop(sem_mut_id, &unlock_connect, 1);
        printf("%s\n", buff);
        sleep(1);
    }
}


void *snd_handler(void *arg)
{
    char msg[MAX_MSG + MAX_NAME + 4];
    char *line;
    while (1) {
        line = _getline(NULL, MAX_MSG);
        sprintf(msg, "[%s]: %s", name, line);
        strcpy(buff, msg);
    }
    
}


int main()
{
    int shmid;
    key_t key;
    
    key = ftok("./server.out", 'A');
    shmid = shmget(key, MAX_MSG * sizeof(char), 0);
    buff = shmat(shmid, NULL, 0);
    
    /* Подключение к бинарному семафору */
    key = ftok("./client.out", 'A');
    sem_mut_id = semget(key, 2, 0);
    
    /* Подключение к счётчику клиентов */
    key = ftok("./Makefile", 'A');
    sem_user_count_id = semget(key, 1, 0);
    
    write(STDOUT_FILENO, "Write name: ", strlen("Write name: "));
    name = _getline(NULL, MAX_NAME);
    semop(sem_user_count_id, &inc, 1);
    semop(sem_mut_id, &unlock_connect, 2);
    //semop(sem_mut_id, lock_connect, 1);
    
    pthread_t tid_snd, tid_rcv;
    
    /* Создание потоков отправления сообщений и получения сообщений */
    pthread_create(&tid_snd, NULL, &snd_handler, NULL);
    pthread_create(&tid_rcv, NULL, &rcv_handler, NULL);
    
    /* Ожидание завершения работы потоков */
    pthread_join(tid_snd, NULL);
    pthread_join(tid_rcv, NULL);
    
    
    
    return 0;
}



