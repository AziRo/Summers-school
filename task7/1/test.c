#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>


int main()
{
    char str_out[] = "Hello World!\n";
    int str_size = (int)sizeof(str_out)-1;
    char str[str_size];
    int fd = open("test.txt", O_RDWR|O_CREAT|O_TRUNC, 0666);
    if(fd == -1){
        write(0, "failed to open file.\n", 21);
        return -1;
    }
    write(fd, str_out, str_size);
    lseek(fd, 0, SEEK_SET);
    read(fd, str, str_size);
    write(0, str, str_size); 
    
    int fsize = lseek(fd, 0, SEEK_END);
    printf("\nfile size = %i\n", fsize);
    
    char buf;
    for(int i = 0; i < str_size; ++i){
        lseek(fd, -i-1, SEEK_END);
        read(fd, &buf, sizeof(char));
        str_out[i] = buf;
    }
    lseek(fd, 0, SEEK_SET); 
    write(fd, str_out, str_size);
    
    return 0;
}
