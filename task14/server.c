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
char *line = NULL;
int quit = 0;

int sem_mut_id,
    sem_user_count_id,
    sem_user_id;


struct sembuf   lock0[2] = {{0, 0, 0}, {0, 1, 0}}, 
                unlock0 = {0, -1, 0},
                lock1[2] = {{1, 0, 0}, {1, 1, 0}}, 
                unlock1 = {1, -1, 0},
                inc = {0, 1, 0},
                dec = {0, -1, 0};


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


void *connect_handler(void *arg)
{
    int user_count = 0;
    char msg[MAX_MSG];
    
    struct sembuf inc_n[2] = {{0, 0, 0}, {0, 1, 0}};
    
    while (1) {
        
        if (user_count != semctl(sem_user_count_id, 0, GETVAL)) {
            /* Подключение/отключение клиента */
            user_count = semctl(sem_user_count_id, 0, GETVAL);
            sprintf(msg, "[Server]: Количество пользователей: %i", user_count);
            strcpy(buff, msg);
            
            /* Поочерёдное разрешение клиентам на чтение буфера*/
            for (int i = 0; i < user_count; ++i) {
                inc_n[1].sem_op = i + 1;
                semop(sem_user_id, inc_n, 2);
            }
        
        }
        
        usleep(200);
    }
    
    
}


void *msg_handler(void *arg)
{
    
    int user_count = 0;
    char msg[MAX_MSG];
    
    struct sembuf inc_n[2] = {{0, 0, 0}, {0, 1, 0}};
    
    while (1) {
        /* Ожидание захвата обработчика сообщений */
        semop(sem_mut_id, &unlock1, 1);
        
        if (quit) {
            free(line);
            break;
        }
        
        if (!strcmp(buff, "quit")) {
            
            semop(sem_user_count_id, &dec, 1);
        
        } else {
        
            user_count = semctl(sem_user_count_id, 0, GETVAL);
            
            /* Поочерёдное разрешение клиентам на чтение буфера*/
            for (int i = 0; i < user_count; ++i) {
                inc_n[1].sem_op = i + 1;
                semop(sem_user_id, inc_n, 2);
            }
        }
           
        /* Освобождение захвата разделяемой памяти */
        semop(sem_mut_id, &unlock0, 1);
    }
}


int main()
{
    int shmid;
    key_t key;
    
    key = ftok("./server.out", 'A');
    shmid = shmget(key, MAX_MSG * sizeof(char), IPC_CREAT | 0666);
    buff = shmat(shmid, NULL, 0);
    
    /* Создание бинарного семафора */
    key = ftok("./Makefile", 'G');
    sem_mut_id = semget(key, 2, IPC_CREAT | 0666);
    
    /* Создание счётчика клиентов */
    key = ftok("./Makefile", 'A');
    sem_user_count_id = semget(key, 1, IPC_CREAT | 0666);
    
    /* Создание семафора, который показывает активного клиента */
    key = ftok("./Makefile", 'C');
    sem_user_id = semget(key, 1, IPC_CREAT | 0666);
    
    
    /* Инициализация семафоров */
    semctl(sem_user_count_id, 0, SETVAL, 0);
    semctl(sem_mut_id, 0, SETVAL, 0);
    semctl(sem_mut_id, 1, SETVAL, 0); 
    semctl(sem_user_id, 0, SETVAL, 0);
    
    
    pthread_t tid_connect, tid_msg;
    
    /* Создание потоков обработки подключения и получения сообщений */
    pthread_create(&tid_connect, NULL, &connect_handler, NULL);
    pthread_create(&tid_msg, NULL, &msg_handler, NULL);
    
    
    /* Обработка команды выключения сервера */
    while(1) {
    
        line = _getline(NULL, 10);
        
        if (!strcmp(line, "quit")) {
            quit = 1;
            semop(sem_mut_id, lock1, 2);
            semctl(sem_user_id, 0, SETVAL, 0);
            pthread_cancel(tid_connect);
            break;
        }
        
        free(line);
    }
    
    /* Ожидание завершения работы потоков */
    pthread_join(tid_connect, NULL);
    pthread_join(tid_msg, NULL);
    
    shmdt(buff);  
    
    return 0;
}
