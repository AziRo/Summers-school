#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "calc.h"
#include <dlfcn.h>

//освобождение буфера стандартного ввода
void flushstdin()
{
    int c;
    while( (c = fgetc( stdin )) != EOF && c != '\n' );
}


int main()
{
    float operand_a, operand_b;
    int operation, dd_id = 0, ind = 0, plug_ind = 0;
    char plug[1024], plug_name[32];
    
    
    //считывание подключаемых плагинов
    printf("Write plagins:\n");
    scanf("%s",plug);
    flushstdin();
    
    char* fn_names[16];
    void* dd[16];
    float (*fn_ptr[16])(float, float);
   
    //позднее связывание 
    while(true){        
        if(plug[plug_ind] == ',' || plug[plug_ind] == '\0'){
            plug_name[ind] = '\0';
            ind = 0;
            dd[dd_id] = dlopen(plug_name, RTLD_NOW);
        
            if(dd[dd_id] == NULL){
                printf("failure dlopen: %s\n", dlerror());
                return -1;
            }else{
               fn_names[dd_id] = dlsym(dd[dd_id], "fn_name");
               fn_ptr[dd_id] = dlsym(dd[dd_id], fn_names[dd_id]);
               dd_id++;
            }
            if(plug[plug_ind++] == '\0') break;
        }
        plug_name[ind++] = plug[plug_ind++];
    }
        
    while(true){
    
        system("clear");
        //вывод меню
        for(int it = 1; it <= dd_id; ++it){
            printf("%i) %s\n", it, fn_names[it-1]);
        }
        
        //считывание операции и операндов
        printf("write operation\n");
        scanf("%i", &operation);
        flushstdin();
        if(operation > dd_id){
            printf("wrong operation\n");
            continue;
        }
        
        printf("write a and b\n");
        scanf("%f %f", &operand_a, &operand_b);
        printf("answer: %f\n", fn_ptr[operation-1](operand_a, operand_b));
      
        flushstdin();
        
        printf("continue? y/n\n");
        operation = fgetc(stdin);
        if(operation == 'n'){
            break;
            for(int it = 0; it < dd_id; ++it)
                dlclose(dd[it]);
        }
        flushstdin();
    }
   
    return 0;
}
