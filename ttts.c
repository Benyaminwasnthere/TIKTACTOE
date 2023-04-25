#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


#define buff_MAX 225
#define PORT 8000

typedef struct {
    int playero;
    int playerx;
    char usernameo[225];
    char usernamex[225];
} Game;



//make the gamethread function
void *gamethread(void *p) {
    Game *game = p;
    char buff[buff_MAX];

    printf("In game thread...\n");


    bzero(buff, buff_MAX);
    buff[0] = 'X';
    buff[1] = '|';
    strcat(buff, game->usernameo);
    strcat(buff, "|");
    int length = strlen(buff);

    char command[buff_MAX];
    strcpy(command, "BEGN|");

    while (length > 0) {
        int digit = length % 10;
        length = length / 10;
        char temp[2];
        sprintf(temp, "%d", digit);
        strcat(command, temp);
    }
    strcat(command, buff);

    write(game->playerx, command, sizeof(command));

    bzero(buff, buff_MAX);
    buff[0] = 'O';
    buff[1] = '|';
    strcat(buff, game->usernamex);
    strcat(buff, "|");
    length = strlen(buff);

    strcpy(command, "BEGN|");
    while (length > 0) {
        int digit = length % 10;
        length = length / 10;
        char temp[2];
        sprintf(temp, "%d", digit);
        strcat(command, temp);
    }
    strcat(command, buff);

    write(game->playero, command, sizeof(command));

    int current_player = 0;









}

int server(void* p) {
    Game *game;
    int sock,clientO,clientX;

    char clientO_username[225], clientX_username[225];

    struct sockaddr_in servaddr, clientinfo;
    printf("In server thread...\n");
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock ==-1){
        printf("socket creation failed...\n");
        return EXIT_FAILURE;
    }
    printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if(bind(sock,(struct sockaddr*)&servaddr,sizeof(servaddr))!=0){
        printf("socket bind failed...\n");
        return EXIT_FAILURE;
    }
    printf("Socket successfully binded..\n");

    if(listen(sock,5)!=0){
        printf("Listen failed...\n");
        return EXIT_FAILURE;
    }
    printf("Server listening..\n");


    //create a while loop to accept clients, wait for 2 clients to send PLAY, then create a thread to handle the game
    int flag = 0;
    int clientO_placeholder;
    int clientX_placeholder;

    while (1) {
        //wait for a client to connect and send PLAY command
        socklen_t size = sizeof(clientinfo);
        clientO = accept(sock, (struct sockaddr *) &clientinfo, &size);
        if (clientO < 0) {
            printf("server accept failed...\n");
            return EXIT_FAILURE;
        }
        printf("server accept the client...\n");
        //read the first 4 bytes to see if it is PLAY
        char buffer[5];
        read(clientO, buffer, 4);
        if (strcmp(buffer, "PLAY") != 0) {
            printf("Client did not send PLAY command\n");
            close(clientO);
            continue;
        }
            //read in the next character, which should be a |
        char c;
        read(clientO, &c, 1);
        if (c != '|') {
            printf("Client did not send PLAY command\n");
            close(clientO);
            continue;
        }
        printf("Client sent PLAY command\n");
        //read in an int, which should be the length of the username
        int usernameLength;
        while (read(clientO, &c, 1) == 1) {
            if (c == '|') {
                break;
            }
            usernameLength = usernameLength * 10 + (c - '0');
        }

        char username[usernameLength + 1];
        read(clientO, username, usernameLength);

        //send the WAIT command
        write(clientO, "WAIT|0|", 7);

        //add the client to the game
        if (flag == 0) {
            clientO_placeholder = clientO;
            strcpy(clientO_username, username);
            flag = 1;
            printf("Client O added to game: %s\n", username);
        } else {
            clientX_placeholder = clientO;
            strcpy(clientX_username, username);
            flag = 0;
            printf("Client X added to game: %s\n", username);
            //create a thread to handle the game
            game = malloc(sizeof(Game));

            pthread_t gameThread;
            pthread_create(&gameThread, NULL, gamethread, game);
        }
    }
}


int main(){
    pthread_t serverThread;
    pthread_create(&serverThread, NULL, server, NULL);
    pthread_join(serverThread, NULL);
}