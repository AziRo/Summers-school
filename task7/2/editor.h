#ifndef __NCURS__H
#define __NCURS__H 


#include <ncurses.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


#define HEIGHT 20
#define WIDTH 40
#define START_X 2
#define START_Y 2

#define HEIGHT_I 5
#define WIDTH_I 40
#define START_X_I 2
#define START_Y_I 23

#define HEIGHT_E 3
#define WIDTH_E 40
#define START_X_E 2
#define START_Y_E 29

#define SIZE_FIELD (WIDTH-1) * (HEIGHT-2) + 1

#define LINE_BREAK 1
#define END_OF_FIELD 2

#define ENTER 10
#define ESC 27


int move_cursor(WINDOW*, int*, int*);

int print_on_win(WINDOW*, char*, int);

int clear_win(WINDOW*, int, char*);

int read_file(int, char*);

int fill_str(char*, int, int,  char);

int print_info(WINDOW*, int);

int print_error(WINDOW* , int);


#endif //__HCURS__H

