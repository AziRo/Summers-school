#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <malloc.h>
#include <unistd.h>


#define N 5
#define CONS_NUMBER 3
#define MAX_GOODS_COUNT 250


int warehouse[N];
int is_ready;
pthread_mutex_t mut[N];


struct consumer{
    int amount_needs;
    int number;
};

void* loader(void *arg)
{
    int stat;
    /* Пока все потребители не будут удовлетворены */
    while (is_ready != CONS_NUMBER) {
         /* Проход по всем складам */
         for (int i = 0; i < N; ++i) {
            
            stat = pthread_mutex_trylock(&mut[i]);
            /* Если склад свободен */
            if (!stat) {
                printf("Погрузчик прибыл на %i склад.\n", i + 1);
                sleep(2);
                warehouse[i] += 500;
                printf("Погрузчик загрузил на %i склад %i единиц товара.\n", i + 1, 500);
                printf("На %i складе %i единиц товара.\n", i + 1, warehouse[i]);
                
                pthread_mutex_unlock(&mut[i]);
             
                sleep(2);
                printf("Погрузчик покинул %i склад.\n", i + 1);
                sleep(2);
            }
        
        }
        
        sleep(2);
    }
}


void* consumer(void *arg)
{
    struct consumer cons = *(struct consumer*)arg;
    int stat;
    int cons_n = cons.number, need = cons.amount_needs, goods_count;
    /* До тех пор, пока не будут удовлетворены все потребности */
    do {
        /* Проход по всем складам */
        for (int i = 0; i < N; ++i) {
            
            stat = pthread_mutex_trylock(&mut[i]);
            /* Если склад свободен */
            if (!stat) {
            
                printf("Потребитель номер %i прибыл на %i склад c потребностью %i шт.\n",cons_n, i+1, need);
                
                sleep(2);
                
                /* Определение количества товаров, которые возьмёт потребитель */
                if (warehouse[i] < need) {
                    if (warehouse[i] < MAX_GOODS_COUNT)
                        goods_count = warehouse[i];
                    else
                        goods_count = MAX_GOODS_COUNT;
                } else {
                    if (need < MAX_GOODS_COUNT)
                        goods_count = need;
                    else
                        goods_count = MAX_GOODS_COUNT;
                }
                /* Если на складе нет товаров*/
                if (!goods_count) {
                    pthread_mutex_unlock(&mut[i]);
                    printf("Потребитель номер %i покинул %i склад в связи с нехваткой товара.\n", cons_n, i+1);
                    continue;
                }
                
                printf("Потребитель номер %i забрал с %i склада %i единиц товара.\n", cons_n, i+1, goods_count);
                cons.amount_needs -= goods_count;
                warehouse[i] -= goods_count;
                printf("На %i складе осталось %i товара.\n", i+1, warehouse[i]);
                sleep(2);
                
                pthread_mutex_unlock(&mut[i]);
                printf("Потребитель номер %i покинул %i склад.\n", cons_n, i+1);
                sleep(2);
                
                need = cons.amount_needs;
                if(!need) break;
            }
            
            sleep(4);
        }
        
    } while (need);
    printf("Потребитель номер %i удовлетворил все потребности.\n", cons_n);
    ++is_ready;
}

void init_mutex()
{
    for (int i = 0; i < N; ++i) {
        pthread_mutex_init(&mut[i], NULL);
    }
}



int main()
{
    pthread_t tid_loader;
    pthread_t tid_consumers[CONS_NUMBER];
    
    int value, status;
    struct consumer cons[CONS_NUMBER];
    
    for (int i = 0; i < N; ++i) {
        warehouse[i] = 1000;
    }
    
    
    init_mutex();
    /* Создание потока "погрузчика" */
    status = pthread_create(&tid_loader, NULL, &loader, NULL);
    if (status != 0 ) {
        printf("can't create thread: status %i\n", status);
    }
    
    /* Создание потоков "потребителей" */
    for (int i = 0; i < CONS_NUMBER; ++i) {
        cons[i].amount_needs = rand() % 3000 + 8001;
        cons[i].number = i + 1;
        status = pthread_create(&tid_consumers[i], NULL, &consumer, &cons[i]);
        if (status != 0 ) {
            printf("can't create thread: status %i\n", status);
        }
    }
    
    /* Ожидание завершения потоков */
    for (int i = 0; i < CONS_NUMBER; ++i) {
        pthread_join(tid_consumers[i], (void**)&value);
    }
    
    // pthread_cancel(tid_loader);
    pthread_join(tid_loader, (void**)&value);
    
    return 0;
}
