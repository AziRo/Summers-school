#include <stdio.h>
#include <stdlib.h>


int main(){
    int *ptr = NULL;
/*
    for(int i = 0; i < 10; ++i){
        ptr = (int*)malloc(sizeof(int)*100);
    }
    */printf("%i", ptr[1000]);
    free(ptr);
    
    return 0;
}
