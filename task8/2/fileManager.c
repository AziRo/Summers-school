#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>


#define KEY_TAB 9
#define ENTER 10

#define SWITCH 10
#define QUIT 20

#define HEIGHT 30
#define WIDTH 30
#define DELTA_X 2
#define DELTA_Y 2

int clear_win(WINDOW *win, int start, int h, char *clear_str)
{
    for (int i = start; i < h - 1; ++i) {
        wmove(win, i, start);
        wprintw(win, "%s", clear_str);
    }
    
    wmove(win, 1, 1);
    wrefresh(win);
    
    return 0;
}

int clear_w(int start, int h, char *clear_str)
{
    attron(A_STANDOUT);
    for (int i = start; i < h; ++i) {
        move(i, 0);
        printw("%s", clear_str);
    }
    refresh();
    attron(A_STANDOUT);
    
    return 0;
}

int fill_str(char *str, int start, int size,  char c)
{    
    for(int i = start; i < size; ++i){
        str[i] = c;
    }
    str[size] = 0;
    
    return 0;
}

int win_handler(WINDOW* win, char *path)
{
    chdir(path);
    endwin();
    int buf, y = DELTA_Y + 1;
    int row = DELTA_Y + 1, column = DELTA_X + 1;
    int ind, status;
    char clear_str[WIDTH - 2];
    char clear_str2[81];
    fill_str(clear_str, 0, WIDTH - 3, ' ');
    fill_str(clear_str2, 0, 80, ' ');

    struct dirent **dirent;
    struct stat file_stat;    
    int count = scandir(".", &dirent, NULL, alphasort);
    
    /* Отрисовка окна с файлами */
    clear_win(win, 1, HEIGHT, clear_str); 
    clear_w(0, 1, clear_str2);
    move(0, 0);
    printw("%s", path);
    refresh();
                                   
    
    for(int i = 0; i < count; ++i){
        wmove(win, row, column);
        row++; 
        wprintw(win, "%s", dirent[i]->d_name); 
    }
    
    wmove(win, DELTA_Y + 1, column);
    wattron(win, A_STANDOUT);
    wprintw(win, "%s", dirent[0]->d_name);
    wattroff(win, A_STANDOUT);
    wrefresh(win);
    
    /* Обработка нажатия клавиш */
    while(1){
        buf = wgetch(win);
        switch(buf){
            case KEY_UP:
                if(y > DELTA_Y + 1){
                    --y;
                    ind = y - DELTA_Y - 1;
                    wmove(win, y + 1, column);
                    wprintw(win, "%s", dirent[ind + 1]->d_name);
                    
                    wmove(win, y, column);
                    wattron(win, A_STANDOUT);
                    wprintw(win, "%s", dirent[ind]->d_name);
                    wattroff(win, A_STANDOUT);
                   
                    wrefresh(win);
                }
            break;
            case KEY_DOWN:
                if(y < DELTA_Y + HEIGHT - 1){
                    ind = y - DELTA_Y;
                    if(ind >= count){
                        break;
                    }
                    ++y;
                    wmove(win, y - 1, column);
                    wprintw(win, "%s", dirent[ind - 1]->d_name);
                    
                    wmove(win, y, column);
                    wattron(win, A_STANDOUT);
                    wprintw(win, "%s", dirent[ind]->d_name);
                    wattroff(win, A_STANDOUT);
                    
                    wrefresh(win);
                }
            break;
            case ENTER:
                move(0, 0);
                int ind = y - (DELTA_Y + 1);
                refresh();
                stat(dirent[ind]->d_name, &file_stat);
                /* Переход по каталогам */
                if(S_ISDIR(file_stat.st_mode)){
                    row = DELTA_Y + 1;
                    y = DELTA_Y + 1;
                    chdir(dirent[ind]->d_name);
                    getcwd(path, 256);
                    clear_w(1, 1, clear_str2);
                    printw("%s", path);
                    refresh();
                    
                    for(int i = 0; i < count; ++i){
                        free(dirent[i]);
                    }
                    free(dirent);
                    
                    count = scandir(".", &dirent, NULL, alphasort);
                    
                    clear_win(win, 1, HEIGHT, clear_str); 
                    
                    for(int i = 0; i < count; ++i){
                        wmove(win, row, column);
                        row++;
                        wprintw(win, "%s", dirent[i]->d_name); 
                    }
                    
                    wmove(win, DELTA_Y + 1, column);
                    wattron(win, A_STANDOUT);
                    wprintw(win, "%s", dirent[0]->d_name);
                    wattroff(win, A_STANDOUT);
                    wrefresh(win);
                /* Открытие бинарного файла в отдельном процессе*/
                } else {
                    pid_t pid = fork();
                    
                    if (pid == 0) {
                        execl(dirent[ind]->d_name, dirent[ind]->d_name, NULL, NULL);
                    } else if (pid == -1) {
                        refresh();
                        
                    } else {
                        endwin();
                        wait(&status);
                        buf = 0;
                        /*
                        while(buf != ENTER){
                            buf = wgetch(win);
                        }
                        */
                        refresh(); 
                    }
                }
            break;
            
            /* Переход в соседнее окно файлового менеджера */
            case KEY_TAB:
                for(int i = 0; i < count; ++i){
                        free(dirent[i]);
                }
                free(dirent);
                
                return SWITCH;
            
            /* Выход из файлового менеджера */
            case 27:
                for(int i = 0; i < count; ++i){
                    free(dirent[i]);
                }
                free(dirent);
                
                return QUIT;
        }
    
    }

    return 0;
}

int main()
{
    initscr();
    noecho();
    raw();
    
    WINDOW *win = newwin(HEIGHT, WIDTH, DELTA_Y, DELTA_X);
    WINDOW *win2 = newwin(HEIGHT, WIDTH, DELTA_Y, DELTA_X + WIDTH);
    refresh();
    box(win, 0, 0);
    box(win2, 0, 0);
    wrefresh(win);
    wrefresh(win2);
    keypad(win, TRUE);
    keypad(win2, TRUE);
    
    char start_path1[256] = {"/"};
    char start_path2[256] = {"/"};
    
    while(1){ 
        if(win_handler(win, start_path1) == QUIT) break;
        if(win_handler(win2, start_path2) == QUIT) break; 
    }
    
    endwin();
    
    return 0;
}
