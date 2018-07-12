#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;
    int fd_out[2];
    int fd_in[2];
    
    char buf[] = "HELLO1\n";
    int size = strlen(buf);
    char buf2[256];
    
    pipe(fd_in);
    pipe(fd_out);
    pid = fork();
    
    if (pid == 0) {
        close(fd_out[0]);
        close(fd_in[1]);
        
        read(fd_in[0], buf2, size);
        write(1, buf2, size);
        buf2[strlen(buf) - 2] = '2';
        write(fd_out[1], buf2, size);
        
        exit(0);
    } else {
        close(fd_out[1]);
        close(fd_in[0]);
        
        write(fd_in[1], buf, size);
        read(fd_out[0], buf2, size);
        wait(NULL);
        write(1, buf2, size);
    }
    
    system("ls -l | cut -c48-60");
    
    
    return 0;
}
