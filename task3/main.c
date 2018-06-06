#include <stdio.h>

extern int pow_n(int a, int n);


int main()
{
    
    int a, n;

    printf("Write a & n\n");
    scanf("%i %i", &a, &n);
    
    printf("Answer: %i\n", pow_n(a, n));

    return 0;
}
