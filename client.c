#include <arpa/inet.h> 
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> 
#include <sys/socket.h>
#include <unistd.h>
#define MAX_BUFFER 100
#define PORT 8000
void chat(int sock){
     char buff[MAX_BUFFER];
      int n;
      for (;;) {
        bzero(buff, sizeof(buff));
        printf("Enter the string : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;
        write(sock, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
        read(sock, buff, sizeof(buff));
        printf("From Server : %s", buff);
        if ((strncmp(buff, "exit", 4)) == 0) {
            printf("Client Exit...\n");
            break;
        }
    }
}




int main(){
    struct sockaddr_in servaddr;
    int sock;
      sock = socket(AF_INET, SOCK_STREAM, 0);
    //On success, a file descriptor for the new socket is returned.  On
     // error, -1 is returned, and errno is set to indicate the error.
    if( sock ==-1){
        printf("socket creation failed...\n");
        return EXIT_FAILURE;
    }   
    printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr =inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);
    if(connect(sock,(struct sockaddr*)&servaddr,sizeof(servaddr))!=0){
         printf("connection with the server failed...\n");
         return EXIT_FAILURE;
    }
     printf("connected to the server..\n");
    chat(sock);
    close(sock);




}
