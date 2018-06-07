#include <stdio.h>
#include <stdbool.h>

extern int add(int a, int b);
extern int sub(int a, int b);
extern int mult(int a, int b);
extern int pow_b(int a, int b);

extern float div(int a, int b);
extern float log_b(int a, int b);
extern float _sin(int a);
extern float _cos(int a);


void flushstdin()
{
    int c;
    while( (c = fgetc( stdin )) != EOF && c != '\n' );
}


int main()
{
    int a, b;
    char operation;
        
    while(true){
        printf("write operation\n");
        scanf("%c", &operation);
        
        if(operation == 'e') break;
        if(operation == 's' || operation == 'c'){
            printf("write a");
            scanf("%i", &a);
        }
        else{
            printf("write a and b\n");
            scanf("%i %i", &a, &b);
        }
        
        switch(operation){
            case '+':
                printf("answer: %i\n", add(a, b));
            break;
            case '-':
                printf("answer: %i\n", sub(a, b));
            break;
            case '*':
                printf("answer: %i\n", mult(a, b));
            break;
            case '/':
                printf("answer: %f\n", div(a, b));
            break;
            case 'l':
                printf("answer: %f\n", log_b(a,b));
            break;
            case 's':
                printf("answer: %f\n", _sin(a));
            break;
            case 'c':
                printf("answer: %f\n", _cos(a));
            break;
            case 'p':
                printf("answer: %i\n", pow_b(a,b));
            break;
            default:
                printf("wrong operation\n");
        }
        flushstdin();
    }
    
    return 0;
}
