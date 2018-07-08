#include "fileManager.h"


int main()
{
    initscr();
    noecho();
    raw();
    
    WINDOW *win = newwin(HEIGHT, WIDTH, DELTA_Y, DELTA_X);
    WINDOW *win2 = newwin(HEIGHT, WIDTH, DELTA_Y, DELTA_X + WIDTH);
    WINDOW *info_win = newwin(HEIGHT_I, WIDTH_I, DELTA_Y + HEIGHT, DELTA_X);
    refresh();
    box(win, 0, 0);
    box(win2, 0, 0);
    box(info_win, 0, 0);
    wrefresh(win);
    wrefresh(win2);
    wrefresh(info_win);
    keypad(win, TRUE);
    keypad(win2, TRUE);
    
    print_info(info_win);
    attron(A_STANDOUT);
    
    char start_path1[256] = {"/"};
    char start_path2[256] = {"/"};
    
     
    while (1) { 
        if(win_handler(win, start_path1, start_path2) == QUIT) break;
        if(win_handler(win2, start_path2, start_path1) == QUIT) break; 
    }
    
    endwin();
    
    return 0;
}


int clear_win(WINDOW *win, int start, int h, char *clear_str)
{
    /* Проход по строкам окна */
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
    /* Проход по строкам главного окна */
    for (int i = start; i < h; ++i) {
        move(i, 0);
        printw("%s", clear_str);
    }
    refresh();
    
    return 0;
}

int fill_str(char *str, int size,  char c)
{   
    /* Проход по элементам строки */
    for(int i = 0; i < size; ++i){
        str[i] = c;
    }
    str[size] = 0;
    
    return 0;
}


void* copy(void *arg)
{
    struct copy_info *icopy = arg;
    char *filename = icopy->filename;
    char *path = icopy->path;
    char buff;
    
    mvprintw(37, 1, "Copy %s in %s", filename, path);
    refresh();
    
    /* Подготовка файлов для копирования */
    int fd = open(filename, O_RDWR, 0666);
    chdir(path);
    int out_fd = open(filename, O_WRONLY | O_CREAT, 0666);
    
    int fsize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    
    if(fsize == 0) return NULL;
    float part_progress = 100 / (float)fsize;
    
    progress = 0;
    /* Копирование файла */
    for (int i = 0; i < fsize; ++i) {
        read(fd, &buff, sizeof(char));       
        write(out_fd, &buff, sizeof(char));
        progress += part_progress;
        pthread_cond_signal(&cond);
    }
    
    progress = 100;
    pthread_cond_signal(&cond);    
    close(fd);
    close(out_fd);
}

void *copy_progress(void *arg)
{
    int last_progress = 0;
    attron(A_STANDOUT);
    int prog;
    /* Отображение прогресса копирования файла по сигналу */
    while (1) {
        pthread_cond_wait(&cond, &mut);
        prog = progress;
        move(39, last_progress + 1); 
        for (int i = last_progress; i < prog; ++i) {
                printw(" ");
                refresh();
        }
        last_progress = prog;
	    mvprintw(40, 1, "%3.2f%%.", progress);
        refresh();
        if (progress == 100) break;   
    }
    /* Отображение остатка прогресса */
    if(prog != 100)
    for (int i = last_progress; i < 100; ++i) {
        printw(" ");
        refresh();
    }
    last_progress = prog;
	mvprintw(40, 1, "%3.2f%%.", progress);
    refresh();
    attroff(A_STANDOUT);
    
}


int win_handler(WINDOW* win, char *path, char *sw_path)
{
    chdir(path);
     
    int buf, y = 1;
    int row = 1, column = 1;
    int ind = 0, status;
    char clear_str[WIDTH - 2];
    char clear_str2[101];
    fill_str(clear_str, WIDTH - 3, ' ');
    fill_str(clear_str2, 100, ' ');
    struct copy_info icopy;
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
    
    wmove(win, 1, column);
    wattron(win, A_STANDOUT);
    wprintw(win, "%s", dirent[0]->d_name);
    wattroff(win, A_STANDOUT);
    wrefresh(win);
    
    /* Обработка нажатия клавиш */
    while(1){
        buf = wgetch(win);
	attroff(A_STANDOUT);
	clear_w(37, 41, clear_str2);
	attron(A_STANDOUT);
        switch(buf){
            case KEY_UP:
                if(y > 1){
                    --y;
                    ind = y - 1;
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
                if(y < HEIGHT - 2){
                    ind = y;
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
                int ind = y - 1;
                refresh();
                stat(dirent[ind]->d_name, &file_stat);
                /* Переход по каталогам */
                if(S_ISDIR(file_stat.st_mode)){
                    row = 1;
                    y = 1;
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
                    
                    wmove(win, 1, column);
                    wattron(win, A_STANDOUT);
                    wprintw(win, "%s", dirent[0]->d_name);
                    wattroff(win, A_STANDOUT);
                    wrefresh(win);
                /* Открытие бинарного файла в отдельном процессе*/
                } else {
                    pid_t pid = fork();
                    
                    if (pid == 0) {
                        execl(dirent[ind]->d_name, dirent[ind]->d_name, NULL, NULL);
                        refresh();
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
            
            case KEY_F(2):
                icopy.path = sw_path;
                ind = y - 1;
                icopy.filename = dirent[ind]->d_name;
                
                progress = 0;
                pthread_create(&tid[1], NULL, &copy, &icopy);
                pthread_create(&tid[0], NULL, &copy_progress, NULL);
                pthread_join(tid[1], NULL);
                pthread_join(tid[0], NULL);
                
                chdir(path);      
                break;
            
            /* Переход в соседнее окно файлового менеджера */
            case KEY_TAB:
                for(int i = 0; i < count; ++i){
                        free(dirent[i]);
                }
                free(dirent);
                
                return SWITCH;
            
            /* Выход из файлового менеджера */
            case ESC:
                for(int i = 0; i < count; ++i){
                    free(dirent[i]);
                }
                free(dirent);
                
                return QUIT;
        }
    
    }

    return 0;
}


int print_info(WINDOW *win)
{
    wmove(win, 1, 1);
    
    wattron(win, A_STANDOUT);
    wprintw(win, "ENTER:");
    wattroff(win, A_STANDOUT);
    wprintw(win, " open file        ");
    
    wattron(win, A_STANDOUT);
    wprintw(win, "ESC:");
    wattroff(win, A_STANDOUT);
    wprintw(win, " quit        ");
    
    wattron(win, A_STANDOUT);
    wprintw(win, "TAB:");
    wattroff(win, A_STANDOUT);
    wprintw(win, " switch to another window        ");
    
    wattron(win, A_STANDOUT);
    wprintw(win, "F2:");
    wattroff(win, A_STANDOUT);
    wprintw(win, " copy file");
    
    wrefresh(win);
    
    return 0;
}
