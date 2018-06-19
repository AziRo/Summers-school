#include <stdio.h>


struct test1{
    char a;
    int b;
};


struct test2{
    char a;
    int b;
}__attribute__((packed));


int main()
{
    char str[10] = {'A', 0, 0, 0, 0, 'B', 0, 0, 0, 0};
    struct test1 *ptr1;
    struct test2 *ptr2;

    ptr1 = &str;
    ptr2 = &str;

    printf("size of struct test1: %i\n", sizeof(*ptr1));
    printf("size of struct test2:  %i\n", sizeof(*ptr2));
    printf("not packed: ptr1.a = %c | ptr1.b = %i\n ", ptr1->a, ptr1->b);
    printf("packed: ptr2.a = %c | ptr2.b = %i\n", ptr2->a, ptr2->b);

    return 0;
}
