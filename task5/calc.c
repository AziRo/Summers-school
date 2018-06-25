#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "calc.h"

//освобождение буфера стандартного ввода
void flushstdin()
{
    int c;
    while( (c = fgetc( stdin )) != EOF && c != '\n' );
}

//Вывод информационного меню
int print_menu()
{
    system("clear");
    printf("'+' : add\n");
    printf("'-' : sub\n");
    printf("'*' : mul\n");
    printf("'/' : div\n");
    printf("'l' : log_b\n");
    printf("'s' : sin\n");
    printf("'c' : cos\n");
    printf("'p' : pow_b\n");
    printf("'e' : exit\n\n");
    
    return 0;
}


int main()
{
    int operand_a, operand_b;
    char operation;
        
    while(true){
        print_menu();
        printf("write operation\n");
        //считывание операции и операндов
        operation = fgetc(stdin);
        if(operation == 'e') break;
        if(operation == 's' || operation == 'c'){
            printf("write a");
            scanf("%i", &operand_a);
        }
        else{
            printf("write a and b\n");
            scanf("%i %i", &operand_a, &operand_b);
        }
        //Выполнение операций
        switch(operation){
            case '+':
                printf("answer: %i\n", add(operand_a, operand_b));
            break;
            case '-':
                printf("answer: %i\n", sub(operand_a, operand_b));
            break;
            case '*':
                printf("answer: %i\n", mult(operand_a, operand_b));
            break;
            case '/':
                printf("answer: %f\n", _div(operand_a, operand_b));
            break;
            case 'l':
                printf("answer: %f\n", log_b(operand_a, operand_b));
            break;
            case 's':
                printf("answer: %f\n", _sin(operand_a));
            break;
            case 'c':
                printf("answer: %f\n", _cos(operand_a));
            break;
            case 'p':
                printf("answer: %i\n", pow_b(operand_a, operand_b));
            break;
            default:
                printf("wrong operation\n");
        }
        
        flushstdin();
        
        printf("continue? y/n\n");
        operation = fgetc(stdin);
        if(operation == 'n') break;
        
        flushstdin();
    }
    
    return 0;
}
