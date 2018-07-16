#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>


int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in saddr, caddr;
    
    bzero(&saddr, sizeof(saddr));
    bzero(&saddr, sizeof(caddr));
    
    saddr.sin_family = AF_INET;
    saddr.sin_port = 56;
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    caddr.sin_family = AF_INET;
    caddr.sin_port = 10;
    caddr.sin_addr.s_addr = inet_addr("127.0.0.2");
    
    bind(fd, (struct sockaddr *) &caddr, sizeof(caddr));
    
    char buf[256];
    int size_dest = sizeof(saddr);
    
    connect(fd, (struct sockaddr *) &saddr, size_dest);
    
    strcpy(buf, "Hello server!");
    send(fd, buf, strlen(buf), 0);
	recv(fd, buf, 256, 0);
    printf("%s\n", buf);
        
    close(fd);
    
    return 0;
}
