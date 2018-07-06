#include <stdio.h>
#include <pthread.h>
#include <malloc.h>


void* print_num(void *arg)
{
    int *num = arg;
    printf("number of thread: %i\n", *num);
    
    return *num;
}



int main()
{
    pthread_t tid[5];
    int num[5];
    int value, status;
    
    for(int i = 0; i < 5; ++i){
        num[i] = i+1;
        status = pthread_create(&tid[i], NULL, &print_num, &num[i]);
        if(status != 0 ){
            printf("can't create thread. Status %i\n", status);
        }
    }
    
    for(int i = 0; i < 5; ++i){
        pthread_join(tid[num[i] - 1], (void**)&value);
        printf("1) returned value: %d\n", value);
    }
        
    printf("---------end----------\n");
    
    return 0;
}
