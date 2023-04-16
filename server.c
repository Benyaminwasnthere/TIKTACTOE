#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#define PORT 8000
#define MAX_BUFFER 100
void chat(int clientsock){
    char buff[MAX_BUFFER];
    int n;
     for (;;) {
        bzero(buff, MAX_BUFFER);
        read(clientsock, buff, sizeof(buff));
        printf("From client: %s\t ", buff);
        bzero(buff, MAX_BUFFER);
        n = 0;
        while ((buff[n++] = getchar()) != '\n');
        write(clientsock, buff, sizeof(buff));    


        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
     }
}
void* server (void*p){
    int rv=0;
     int sock,clientsock;
    struct sockaddr_in servaddr, clientinfo;
    printf("In server thread...\n");
    sock = socket(AF_INET, SOCK_STREAM, 0);
    //On success, a file descriptor for the new socket is returned.  On
     // error, -1 is returned, and errno is set to indicate the error.
    if( sock ==-1){
        printf("socket creation failed...\n");
        return NULL;
    }   
    printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    // On success, zero is returned.  On error, -1 is returned, and errno is set to indicate the error.
    if(bind(sock,(struct sockaddr*)&servaddr,sizeof(servaddr))!=0){
            printf("socket bind failed...\n");
            return NULL;
    }
    printf("Socket successfully binded..\n");









    for(;;){
        if ((listen(sock, 4)) != 0) {
            printf("Listen failed...\n");
            return NULL;
        }
        printf("Server listening..\n");
        int size=sizeof(clientinfo);
        clientsock=accept(sock,(struct sockaddr*)&clientinfo,&size);
        if(clientsock<0){
            printf("server accept failed...\n");
            return NULL;
        }
        printf("server accept the client...\n");
        
        chat(clientsock);
        close(clientsock);
    
    }
    pthread_exit(&rv);
}


int main(){
    int *ptr;
    pthread_t thr;
    pthread_create(&thr,NULL,server,NULL);
    pthread_join(thr,(void**)&ptr);
    return EXIT_SUCCESS;
}
