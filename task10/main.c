#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main()
{
    pid_t main_pid = getpid();
    printf("This is main process | pid = %i\n", main_pid);
    
    pid_t pid[5];
    int status;

    pid[0] = fork();
     
    switch (pid[0]) { 
        case 0:
            printf("This is process #1 | pid = %i <- ppid = %i\n", getpid(), getppid());
            
            pid[1] = fork();
            
            switch (pid[1]) {
                case 0:
                    printf("This is process #2 | pid = %i <- ppid = %i\n", getpid(), getppid());
                    exit(0);
                default:
                    printf("This is parent | pid = %i -> cpid = %i\n", getpid(), pid[1]);
            }
            
            exit(0);    
        default:
            printf("This is parent | pid = %i -> cpid = %i\n", getpid(), pid[0]);
            
            pid[2] = fork();
        
            switch (pid[2]) {
                case 0:
                    printf("This is process #3 | pid = %i <- ppid = %i\n", getpid(), getppid());
                    
                    pid[3] = fork();
                
                switch (pid[3]) {
                    case 0:
                        printf("This is process #4 | pid = %i <- ppid = %i\n", getpid(), getppid());
                        exit(0);
                    default:
                        printf("This is parent | pid = %i -> cpid = %i\n", getpid(), pid[3]);
                }
                
                pid[4] = fork();
                
                switch (pid[4]) {
                    case 0:
                        printf("This is process #5 | pid = %i <- ppid = %i\n", getpid(), getppid());
                        exit(0);
                    default:
                        printf("This is parent | pid = %i -> cpid = %i\n", getpid(), pid[4]);
                }
                
                 exit(0);
            default:
                printf("This is parent | pid = %i -> cpid = %i\n", getpid(), pid[2]);
        }
    }
    
    for(int i = 0; i < 5; ++i)
    {
        wait(&status);
    }
    
    return 0;
}
