#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <malloc.h>


#define MAX_COM 8
#define MAX_COM_LEN 64


char **_getline(int *com_count)
{
    char **com = malloc(MAX_COM * sizeof(char*));
    
    for (int i = 0; i < MAX_COM; ++i) {
        com[i] = malloc(MAX_COM_LEN);
    }
    int i;
    for (i = 0; i < MAX_COM; ++i) {
        
        *com_count = i + 1;
        
        for (int j = 0; j < MAX_COM_LEN; ++j) {
            
            read(STDOUT_FILENO, &com[i][j], 1);
            printf("%c", com[i][j]);
            if (com[i][j] == '|') {
                com[i][j] = 0;
                break;
            }
            
            if (com[i][j] == '\n') {
                com[i][j] = 0;
                i = MAX_COM;
                break;
            }
        }
    
    }
       
    return com;
}


void _free(char **buf, int row_count)
{
    for (int i = 0; i < row_count; ++i) {
        free(buf[i]);
    }
    
    free(buf);
}


void pipeline(char **com, int com_count, int *fd, int start)
{
    pid_t pid;
    
    /*
    pipe(fd[com_count - 1]);
    
    int fd1[2]; //пишем (предыдущий, созданный дескриптор)
    if (!start) {
        fd1[0] = fd[com_count][0];
        fd1[1] = fd[com_count][1];
    }
    
    int fd2[2]; //считываем (текущий дескриптор)
    fd2[0] = fd[com_count - 1][0];
    fd2[1] = fd[com_count - 1][1];
    */
    if(com_count == 1) pipe(fd);
    pid = fork();
    
    //int std_fd = dup(1);
    if (pid == 0) {
        
        if (com_count > 1) {
            pipeline(com, com_count - 1, fd, 0);
            //wait(NULL);
            
            dup2(fd[0], 0);
            //close(fd[0]);
            
        } else {
        
            close(fd[0]);
        
        }
        
        if (!start) {
             
            dup2(fd[1], 1);
             //close(fd[1]);
        
        } else {
            
            close(fd[1]);
            
        }
        //write(std_fd, "com: ", 5);
        //write(std_fd, com[com_count - 1], strlen(com[com_count - 1]));
        //write(std_fd, "\n", 1);
        system(com[com_count - 1]);
        
       
        exit(0);
        
    } else {
        //wait(NULL);
        //write(std_fd, "next\n", 5);
        return;
    }
   
}


int main()
{
    pid_t pid;
    int fd_out[2];
    int fd_in[2];
    
    //char buf[] = "ls -l";
    
    //int size = strlen(buf);
    
    int com_count;
    char **com = _getline(&com_count);
    
    //char buf[256];
    
    
    
    
   /* pipe(fd_in);
    pid = fork();
    if (pid == 0) {
        close(fd_in[1]);
        
        //read(fd_in[0], buf2, size);
        //char buf3[] = "cut -c48-60";
        
       // int size1 = strlen(buf);
        
        pipe(fd_out);
        pid_t pid1 = fork();
        if (pid1 == 0) {
            close(fd_out[0]);
            dup2(fd_out[1], 1);
            system(com[0]);
            exit(0);
        } else {
            close(fd_out[1]);
            dup2(fd_out[0], 0);
            wait(NULL);
            system(com[1]);
            exit(0);
        }
    } else {
        close(fd_in[0]);
        //write(fd_in[1], com[0], size);
        wait(NULL);
        _free(com, MAX_COM);
    }
 */   
   
   /* puts("");
    for(int i = 0; i < com_count; ++i){
        printf("com[%i] = %s\n", i, com[i]);
    }
    
    int **fd;
    
    fd = malloc(com_count * sizeof(int*));
    for (int i = 0; i < com_count; ++i) {
        fd[i] = malloc(2 * sizeof(int));
    }
    */
    
    pipeline(com, com_count, fd_in, 1);
    
    _free(com, MAX_COM);
    
    return 0;
}
