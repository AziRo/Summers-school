#include "editor.h"


int main()
{   
    int buf, key, fn_ind = 0, fd, str_ind = 0;
    int dx = 1, dy = 1, size_str_out = 0;
    char filename[32], str_out[SIZE_FIELD];
    int size_fn = sizeof(filename) - 1;
    int flength = 0;
    char clear_str[WIDTH - 2];
    
    fill_str(clear_str, 0, WIDTH_I - 2, ' ');
    
    initscr();
    
    noecho();
    raw();
    
    //Инициальзация окон
    WINDOW *win = newwin(HEIGHT, WIDTH, START_Y, START_X);
    WINDOW *win_info = newwin(HEIGHT_I, WIDTH_I, START_Y_I, START_X_I);
    WINDOW *win_err = newwin(HEIGHT_E, WIDTH_E, START_Y_E, START_X_E);
    refresh();
    box(win, 0, 0);
    box(win_info, 0, 0);
    box(win_err, 0, 0);
    wrefresh(win);
    wrefresh(win_info);
    wrefresh(win_err);
    wmove(win, dx, dy);
    print_info(win_info, 0);
    keypad(win, TRUE);
    
    wattron(win, A_STANDOUT);
    
    while(1){
        key = 0;
        buf = wgetch(win);
        print_error(win_err, 3);
        switch(buf){
        //Перемещение каретки
            case KEY_RIGHT: 
                if(dx < WIDTH - 2){
                    ++dx;
                    str_ind++;
                    
                }
                wmove(win, dy,dx);
                key = 1;
            break;
            case KEY_LEFT:
                if(dx > 1){
                    --dx;
                    str_ind--;
                }
                wmove(win, dy,dx);
                key = 1;
            break;
            case KEY_UP: 
                if(dy > 1){
                    --dy;
                    str_ind -= (WIDTH - 3);
                }    
                wmove(win, dy,dx);
                key = 1;    
            break;
            case KEY_DOWN:
                if(dy < HEIGHT - 2){
                    ++dy;
                    str_ind += (WIDTH - 3);
                }
                wmove(win, dy,dx);
                key = 1;
            break;
            //Открытие и чтение файла по введённому имени
            case KEY_F(2):
                print_info(win_info, 1);
                fn_ind = 0;
                //Ввод имени файла
                while(1){
                    buf = getch();
                    //Открытие и чтение файла
                    if(buf == ENTER || fn_ind >= size_fn)
                    {
                        filename[fn_ind] = 0;  
                        print_info(win_info, 0);
                        //Имя не написано
                        if(fn_ind == 0){
                            print_error(win_err, 0);
                            break;
                        }
                        fd = open(filename, O_RDWR, 0666);
                        //Ошибка открытия файла
                        if(fd == -1){
                            print_error(win_err, 1);
                            break;
                        }
                        //Чтение и вывод файла
                        else{
                            flength = read_file(fd, str_out);
                            clear_win(win, HEIGHT, clear_str);
                            print_on_win(win, str_out, flength - 1);
                            fill_str(str_out, flength, SIZE_FIELD, ' ');
                            str_ind = 0;
                            dx = dy = 1;
                            size_str_out = flength;
                        }
                        break;
                    }
                    wprintw(win_info, "%c", buf);
                    wrefresh(win_info);
                    filename[fn_ind++] = buf;
                }
                key = 1;
            break;
            //Сохранение файла
            case KEY_F(3):
                key = 1;
                if (fn_ind != 0 && fd != -1) {
                    write(fd, str_out, size_str_out);
                    lseek(fd, 0, SEEK_SET);
                }
                //Имя файла не введено
                else {
                   print_error(win_err, 2);
                }
            break;    
        }
        //Выход из программы
        if(buf == ESC) break;
        
        //Ввод текста
        if(!key){
            wprintw(win,"%c", buf);
            wrefresh(win);
            str_out[str_ind] = (char) buf;
            
            if (move_cursor(win, &dy, &dx) != END_OF_FIELD) {
                ++str_ind;
                if(str_ind > size_str_out) 
                    size_str_out = str_ind;
            }
        } 
    
    }
   
    wattroff(win, A_STANDOUT);
    
    endwin();
                
    return 0;
}


int move_cursor(WINDOW *win, int *dy, int *dx)
{ 
    if ( ((*dx) == WIDTH - 2) && ((*dy) < HEIGHT - 2) ) {
        *dx = 1;
        ++(*dy);
        wmove(win, *dy, *dx);
        return LINE_BREAK;    
    } 
    else if ((*dx) < WIDTH - 2) {
        ++(*dx);
    } 
    else {
        wmove(win, *dy, *dx);
        return END_OF_FIELD;
    }
      
    return 0;
}

int print_on_win(WINDOW *win, char *str, int size)
{
    int row = 1;
    wmove(win, row, 1);
    wattroff(win, A_STANDOUT);
    
    for (int i = 0; i < size; ++i) {
        if (i == WIDTH - 4) {
            ++row;
            wmove(win, row, 1);
        }
        wprintw(win, "%c", str[i]);
    }
    
    wrefresh(win);
    
    wmove(win, 1, 1);
    wattron(win, A_STANDOUT);

    return 0;
}

int clear_win(WINDOW *win, int h, char *clear_str)
{
    wattroff(win, A_STANDOUT);
    
    for (int i = 1; i < h - 1; ++i) {
        wmove(win, i, 1);
        wprintw(win, "%s", clear_str);
    }
    
    wmove(win, 1, 1);
    wrefresh(win);
    wattron(win, A_STANDOUT); 
    
    return 0;
}

int read_file(int fd, char* str)
{
    char buffer;
    int f_length = 0, str_len = SIZE_FIELD;
   
    while( read(fd, &buffer, sizeof(char)) ){
        if(f_length >= str_len - 1) break;
        str[f_length++] = buffer;
    }
    str[f_length] = 0;
    lseek(fd, 0, SEEK_SET);
    
    return f_length;
}

int fill_str(char *str, int start, int size,  char c)
{    
    for(int i = start; i < size; ++i){
        str[i] = c;
    }
    str[size] = 0;
    
    return 0;
}

int print_info(WINDOW *win_info, int flag)
{   
    char clear_str[WIDTH_I - 2];
    
    fill_str(clear_str, 0, WIDTH_I - 2, ' ');
    clear_win(win_info, HEIGHT_I, clear_str);
    
    wattroff(win_info, A_STANDOUT);
    if (!flag) {
        wmove(win_info, 2,1);
        wprintw(win_info, "F2-Open file; F3-Save file; ESC-Quit.");
    }
    else {
        wmove(win_info, 1, 1);
        wprintw(win_info, "WRITE THE FILE NAME AND PRESS \"ENTER\":");
        wmove(win_info, 3, 1);
    }
    
    wrefresh(win_info);
    wattron(win_info, A_STANDOUT);
    
    return 0;
}

int print_error(WINDOW *win, int err)
{
    char clear_str[WIDTH_E - 2];
    
    fill_str(clear_str, 0, WIDTH_I - 2, ' ');
    clear_win(win, HEIGHT_E, clear_str);
    
    wmove(win, 1, 1);
    wattroff(win, A_STANDOUT);
    switch(err){
        case 0:
            wprintw(win, "File name is empty.");
        break;
        case 1:
            wprintw(win, "Failed to open file.");
        break;    
        case 2:
            wprintw(win, "Write the file name!");
        break;
    }
    wrefresh(win);
    wattron(win, A_STANDOUT);
    
    return 0;
}

