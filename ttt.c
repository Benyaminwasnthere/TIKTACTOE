#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_BUFFER 225
#define PORT 8000

char player;

int main() {
    struct sockaddr_in servaddr;
    int sock;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if( sock ==-1){
        printf("socket creation failed...\n");
        return 1;
    }
    printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr =inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    if(connect(sock,(struct sockaddr*)&servaddr,sizeof(servaddr))!=0){
        printf("connection with the server failed...\n");
        return 1;
    }
    printf("connected to the server..\n");
    char buff[MAX_BUFFER];


    printf("Enter your username: ");
    fgets(buff, MAX_BUFFER, stdin);
    //format the command to send to the server to PLAY|userlen|username
    char command[MAX_BUFFER];
    //set the command to PLAY
    strcpy(command, "PLAY|");
    //add the length of the username
    int userlen = strlen(buff);
    //convert the length to a string
    while (userlen > 0) {
        char digit = userlen % 10;
        userlen /= 10;
        char digitstr[2];
        digitstr[0] = digit + '0';
        digitstr[1] = '\0';
        strcat(command, digitstr);
    }
    //add the username
    strcat(command, "|");
    strcat(command, buff);

    write(sock, command, sizeof(command));

    bzero(buff, sizeof(buff));

    read(sock, buff, 4);
    buff[4] = '\0';
    printf("%s", buff);

    //wait until the server sends a command that starts with BEGN
    while (strncmp(buff, "BEGN", 4) != 0) {
        bzero(buff, sizeof(buff));
        read(sock, buff, MAX_BUFFER);

        printf("%s\n", buff);
    }

    player = buff[5];
    printf("You are player %c\n", player);



}
