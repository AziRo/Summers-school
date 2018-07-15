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


char **_getline(int *com_count, char **args)
{
    int arg = 0;

    char **com = malloc(MAX_COM * sizeof(char*));

    for (int i = 0; i < MAX_COM; ++i) {
        com[i] = malloc(MAX_COM_LEN);
    }

    for (int i = 0; i < MAX_COM; ++i) {

        *com_count = i + 1;
        arg = 0;

        for (int j = 0; j < MAX_COM_LEN; ++j) {

            read(STDOUT_FILENO, &com[i][j], 1);

            /* Разделение команд */
            if (com[i][j] == '|') {
                com[i][j] = 0;
                args[i][arg] = 0;
                break;
            }
            /* Конец ввода строки */
            if (com[i][j] == '\n') {
                com[i][j] = 0;
                args[i][arg] = 0;
                i = MAX_COM;
                break;
            }
            /* Разделение команды и аргументов */
            if ( !arg && j > 1 && com[i][j] == '-' && com[i][j-1] == ' ') {
                args[i][arg++] = com[i][j];
                --j;
            } else if (arg) {
                args[i][arg++] = com[i][j];
                --j;

            }

        }

    }
    /* Удаление проделов в конце команд */
    for (int i = 0; i < *com_count; ++i) {
        for(int j = 0; j < strlen(com[i]); ++j)
            if (com[i][j] == ' ') {
                com[i][j] = 0;
                    break;
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


void _pipeline(char **com, char **args, int com_count)
{
    int fd[2];
    pid_t pid;

    for (int i = 0; i < com_count; ++i) {

        pipe(fd);
        pid = fork();

        if (pid == 0) {
            /* Замена стандартного дескриптока и закрытие не используемых */
            if (i != com_count -1) {
                dup2(fd[1], 1);
                close(fd[0]);
            } else {
                close(fd[0]);
                close(fd[1]);
            }

            /* Вызов команды с агрументами и без */
            if (args[i][0] == 0)
                execlp(com[i], com[i], NULL);
            else
                execlp(com[i], com[i], args[i], NULL);

        } else {

            wait(NULL);
            /* Замена стандартного дескриптока и закрытие не используемых */
            if(i != com_count - 1) {
                dup2(fd[0], 0);
                close(fd[1]);
            } else {
                close(fd[1]);
                close(fd[0]);
            }

        }
    }

}


int main()
{
    pid_t pid;
    int fd_out[2];
    int fd_in[2];

    int com_count;

    char **com;
    char **args = malloc(MAX_COM * sizeof(char*));

    for (int i = 0; i < MAX_COM; ++i) {
        args[i] = malloc(MAX_COM_LEN);
    }

    while(1) {
        /* Формирование массива команд и аргументов */
        com = _getline(&com_count, args);

        if(!strcmp(com[0], "quit")) break;

        /* Конвеерное выполнение команд */
        _pipeline(com, args, com_count);

        _free(com, MAX_COM);
    }

    _free(com, MAX_COM);
    _free(args, MAX_COM);

    return 0;
}
