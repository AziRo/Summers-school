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
int id;

int sem_mut_id,
    sem_user_count_id,
    sem_user_id;


struct sembuf   lock0[2] = {{0, 0, 0}, {0, 1, 0}}, 
                unlock0 = {0, -1, 0},
                lock1[2] = {{1, 0, 0}, {1, 1, 0}}, 
                unlock1 = {1, -1, 0},
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
    printf("Your user id: %i\n", id);
    
    while (1) {
    
        /* Ожидание доступа к разделяемой памятиы */
        if(semctl(sem_user_id, 0, GETVAL) == id){
            printf("%s\n", buff);
            
            /* Оповещение о конце работы с разделяемой памятью серверу */
            semctl(sem_user_id, 0, SETVAL, 0);
        }
        
        usleep(200);
    }
}


void *snd_handler(void *arg)
{
    char msg[MAX_MSG + MAX_NAME + 4];
    char *line;
    
    while (1) {
        line = _getline(NULL, MAX_MSG);
        if(strcmp(line, "quit")) {
            sprintf(msg, "[%s]: %s", name, line);
        } else {
            strcpy(msg, line);
        }
        
        /* Захват разделяемой памяти */
        semop(sem_mut_id, lock0, 2);
        strcpy(buff, msg);
        
        /* Захват обработчика сообщений на сервере */
        semop(sem_mut_id, lock1, 2);
        
        free(line);
        if(!strcmp(msg, "quit")){
            break;
        }
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
    key = ftok("./Makefile", 'G');
    sem_mut_id = semget(key, 2, 0);
    
    /* Подключение к счётчику клиентов */
    key = ftok("./Makefile", 'A');
    sem_user_count_id = semget(key, 1, 0);
    
    /* Подключение к семафору, который определяет активного клиента */
    key = ftok("./Makefile", 'C');
    sem_user_id = semget(key, 1, 0);
    
    /* Авторизация и подключение к серверу */
    write(STDOUT_FILENO, "Write name: ", strlen("Write name: "));
    name = _getline(NULL, MAX_NAME);
    semop(sem_user_count_id, &inc, 1);
    id = semctl(sem_user_count_id, 0, GETVAL);
    
    
    pthread_t tid_snd, tid_rcv;
    
    /* Создание потоков отправления сообщений и получения сообщений */
    pthread_create(&tid_snd, NULL, &snd_handler, NULL);
    pthread_create(&tid_rcv, NULL, &rcv_handler, NULL);
    
    /* Ожидание завершения работы потоков */
    pthread_join(tid_snd, NULL);
    pthread_cancel(tid_rcv);
    pthread_join(tid_rcv, NULL);
    
    shmdt(buff);
    
    return 0;
}



