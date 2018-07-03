#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
     
/* Прототипы наших крюков. */
static void my_init_hook(void);
static void *my_malloc_hook(size_t, const void *);
static void my_free_hook (void *, const void *);

/* Переменные, сохраняющие изначальные крюки. */
static void * (*old_malloc_hook)(size_t, const void *);
static void (*old_free_hook)(void *, const void *);
     
/* Переопределение инициализирующих крюков из библиотеки C. */
void (* volatile __malloc_initialize_hook) (void) = my_init_hook;

static void my_init_hook(void)
{
    old_malloc_hook = __malloc_hook;
    old_free_hook = __free_hook;
    
    __malloc_hook = my_malloc_hook;
    __free_hook = my_free_hook;
}

static void * my_malloc_hook (size_t size, const void *caller)
{
    void *result;

    result = sbrk(size);
    
    return result;
}

/* Вся ответственность по использованию данного крюка возлагается
   на того, кто будет им пользоваться */
static void my_free_hook (void *ptr, const void *caller)
{
    brk(ptr); 
}


int main()
{
    int *mas;
    mas = malloc(10 * sizeof(int));
    
    for (int i = 0; i < 10; ++i) {
        mas[i] = i+1;
    }
    
    for (int i = 0; i < 10; ++i) {
        printf("%i\n", mas[i]);
    }
    
    free(mas);

    return 0;
}
