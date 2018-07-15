#include <stdio.h>
#include <signal.h>
#include <unistd.h>


void handler(int  signal)
{
    printf("Hello signal!\n");
}


int main()
{
    sigset_t set, old;
    struct sigaction sigact;

    sigfillset(&set);

    sigact.sa_handler = &handler;
    sigact.sa_mask = set;
    sigact.sa_flags = 0;

    sigaction(SIGUSR2, &sigact, NULL);

    sigdelset(&set, SIGUSR2);
    sigprocmask(SIG_BLOCK, &set, &old);

    while(1) {
        pause();
        break;
    }

    return 0;
}
