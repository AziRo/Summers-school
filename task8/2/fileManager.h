#ifndef __FILEMANAGER__H
#define __FILEMANAGER__H 


#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>


#define KEY_TAB 9
#define ENTER 10
#define ESC 27

#define SWITCH 10
#define QUIT 20

#define HEIGHT 30
#define WIDTH 49
#define DELTA_X 2
#define DELTA_Y 2

#define HEIGHT_I 3
#define WIDTH_I 98


float progress;
int cur_num_copy = 0;
pthread_t tid[2];
pthread_cond_t cond;
pthread_mutex_t mut;


struct copy_info
{
    char* filename;
    char* path;
};


int clear_win(WINDOW*, int, int , char*);

int clear_w(int, int, char*);

int fill_str(char*, int,  char);

void *copy(void*);

void *copy_progress(void*);

int win_handler(WINDOW*, char*, char*);

int print_info(WINDOW*);


#endif //__FILEMANAGER__H
