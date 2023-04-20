#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#define X 1
#define O -1
#define PORT 8000
#define MAX_BUFFER 100
struct Gamers{
    int playero;
    int playerx;
}typedef Gamers;







void *chat(void* p){
    char board [3][3] ;
    Gamers *gamers = p;
    char buff[MAX_BUFFER];
    int n;
    int temp_read, temp_write;
    int current_player = 0;
    bzero(&board[0], sizeof(board));
    for (;;) {
        temp_read = current_player == 0? gamers->playerx : gamers->playero;
        temp_write = current_player == 0? gamers->playero : gamers->playerx;
        current_player = current_player == 0? 1: 0;
        bzero(buff, MAX_BUFFER);
        read(temp_read, buff, sizeof(buff));
        printf("From client: %s\t ", buff);
        bzero(buff, MAX_BUFFER);
        n = 0;
        while ((buff[n++] = getchar()) != '\n');
        write(temp_write, buff, sizeof(buff));    


        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }


     }
     return NULL;

}
void* server (void*p){
    Gamers *gamers;
    int rv=0;
     int sock,clientsock,clientsockx;
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





// clientsock is made and is used to update clientsockx, later clientsock is updated again and is client sock0
    int flag = 0;

    for(;;){
        if ((listen(sock, 4)) != 0) {
            printf("Listen failed...\n");
            return NULL;
        }
        printf("Server listening..\n");
        socklen_t size=sizeof(clientinfo);
            


        clientsock=accept(sock,(struct sockaddr*)&clientinfo, &size);
        if(clientsock<0){
            printf("server accept failed...\n");
            return NULL;
        }
        printf("server accept the client...\n");
        if(flag ==0){
            clientsockx=clientsock;
            flag=1;
            printf("User X...\n");
        }else{
            pthread_t thr;
            flag=0;
            gamers= (Gamers*)malloc(sizeof(Gamers));
            gamers->playero=clientsock;
            gamers->playerx=clientsockx;
            printf("User O...\n");
            pthread_create(&thr,NULL,chat,(void*)gamers);
        }
        //chat(clientsock);
        //close(clientsock);
    
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
