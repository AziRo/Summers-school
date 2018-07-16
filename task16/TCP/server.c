#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>


#define N 5


int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in caddr, saddr;
    
    bzero(&saddr, sizeof(saddr));
    bzero(&saddr, sizeof(caddr));
    
    saddr.sin_family = AF_INET;
    saddr.sin_port = 56;
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    bind(fd, (struct sockaddr *) &saddr, sizeof(saddr));
    
    listen(fd, N);
    
    char buf[256];
    int size_dest = sizeof(caddr);
    
    int fd_client = accept(fd, (struct sockaddr *) &caddr, &size_dest);
    
    recv(fd_client, buf, 256, 0);
    printf("%s\n", buf);
    strcpy(buf, "Hello client!");
    send(fd_client, buf, strlen(buf), 0);
    
    close(fd);
    
    return 0;
}
