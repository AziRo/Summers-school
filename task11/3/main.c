#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <malloc.h>

#define THREAD_COUNT 5

#define _HELLO_ 0
#define _BYE_ 1
#define _HOW_ARE_YOU_ 2
#define _ALL_NORMAL_ 3
#define _OKAY_ 4


pthread_key_t key;
pthread_once_t once;


void init_key()
{
    pthread_key_create(&key, NULL);
}


char* info(int arg)
{
    char *buff;
    
    /* Инициализация ключа для создания уникального буфера */
    pthread_once(&once, &init_key);
    
    /* Получаем уникальный буфер потока */
    buff = pthread_getspecific(key);
    if (buff == NULL) {
        buff = malloc(256);
        pthread_setspecific(key, buff);
    }
    
    /* Инициальзируем буфер в зависимости от переданного аргумента */
    switch (arg) {
        
        case _HELLO_:
            strcpy(buff, "Привет!");
            break; 
        case _BYE_:
            strcpy(buff, "Пока.");
            break;
        case _HOW_ARE_YOU_:
            strcpy(buff, "Как ты?");
            break;
        case _ALL_NORMAL_:
            strcpy(buff, "Со мной всё впорядке.");
            break;
        case _OKAY_:
            strcpy(buff, "Хорошо.");
            break;
        default:
            strcpy(buff, "Неопределённое сообщение.");
    }
    
    return buff;
}


void *thread_func(void *arg)
{
    int *thread_ind = arg;
    char *buff;
    
    /* Вызов функции info для всех принимаемых значений */
    for (int i = 0; i < 6; ++i) {
        buff = info(i);
        printf("Thread %i: %s | Address: %x\n", *thread_ind, buff, buff);
    }
    
    free(buff);
}


int main()
{
    int t_index[THREAD_COUNT];
    pthread_t tid[THREAD_COUNT];
    
    /* Создание потоков */
    for (int i = 0; i < THREAD_COUNT; ++i) {
        t_index[i] = i + 1;
        pthread_create(&tid[i], NULL, &thread_func, &t_index[i]);
    }
    
    /* Ожидание завершения работы потоков */
    for (int i = 0; i < THREAD_COUNT; ++i) {
        pthread_join(tid[i], NULL);
    }
    
    return 0;
}
