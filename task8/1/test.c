#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>


int main()
{
    chdir("./");
    DIR *dir = opendir(".");
    
    struct dirent *tmp;
    tmp = readdir(dir);
    tmp = readdir(dir);
    
    while ( (tmp = readdir(dir)) != NULL){
        printf("%s\n", (*tmp).d_name); 
    }
    closedir(dir);
    
    free(tmp);
    printf("||||||||||||||||||||||||||||||||||||||||||||\n");
    chdir("./");
    
    struct dirent **dirent;
    struct dirent **dirent1;
    
    struct stat file_stat;
    
    int count = scandir(".", &dirent, NULL, alphasort);
   
   //Вывод файлов
    printf("\nFILE:\n");
    for(int i = 0; i < count; ++i){
        stat(dirent[i]->d_name, &file_stat);
        if(S_ISREG(file_stat.st_mode))
            printf("%s\n", dirent[i]->d_name); 
    }
    //Вывод директорий
    printf("\nDIR:\n");
    for(int i = 0; i < count; ++i){
        stat(dirent[i]->d_name, &file_stat);
        if(S_ISDIR(file_stat.st_mode)){
            printf("%s\n", dirent[i]->d_name); 
        }
    }
    
    for(int i = 0; i < count; ++i){
        free(dirent[i]);
    }
    free(dirent);
    
    return 0;
}
