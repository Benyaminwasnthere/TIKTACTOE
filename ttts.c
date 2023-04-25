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
    char usernameo[200];
    char usernamex[200];
} Game;


int isGameOver(char board[3][3]) {
    //check for horizontal wins
    for (int i = 0; i < 3; i++) {
        if (board[i][0] != '.' && board[i][0] == board[i][1] && board[i][1] == board[i][2]) {
            return 1;
        }
    }
    //check for vertical wins
    for (int i = 0; i < 3; i++) {
        if (board[0][i] != '.' && board[0][i] == board[1][i] && board[1][i] == board[2][i]) {
            return 1;
        }
    }
    //check for diagonal wins
    if (board[0][0] != '.' && board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
        return 1;
    }
    if (board[0][2] != '.' && board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
        return 1;
    }
    //check for tie
    int tie = 1;
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == '.' || board[i][1] == '.' || board[i][2] == '.') {
            tie = 0;
        }
    }
    if (tie == 1) {
        return 2;
    }
    return 0;
}

//make the gamethread function
void *gamethread(void *p) {
    Game *game = p;
    char buff[buff_MAX];

    printf("In game thread...\n");



    /********************
      SEND BEGN COMMANDS
    *******************/
    bzero(buff, buff_MAX);

    buff[strlen(game->usernameo) - 1] = '\0';
    int length = strlen(game->usernameo) +2;
    char command[buff_MAX];
    strcpy(command, "BEGN|");
    char lengthstr[10];

    sprintf(lengthstr, "%d", length);
    strcat(command, lengthstr);
    strcat(command, "|X|");
    strcat(command, game->usernameo);
    command[strlen(command) - 1] = '\0';
    strcat(command, "|");
    command[strlen(command)] = '\0';
    printf("command: %s\n", command);

    write(game->playerx, command, sizeof(command));

    bzero(buff, buff_MAX);
    buff[strlen(game->usernamex) - 1] = '\0';
    length = strlen(game->usernamex) +2;

    strcpy(command, "BEGN|");
    sprintf(lengthstr, "%d", length);
    strcat(command, lengthstr);
    strcat(command, "|O|");
    strcat(command, game->usernamex);
    command[strlen(command) - 1] = '\0';
    strcat(command, "|");
    command[strlen(command)] = '\0';
    printf("command: %s\n", command);

    write(game->playero, command, sizeof(command));

    int current_player = 0;
    int gameover = 0;
    int turn = 0;

    char board[3][3] = {{'.', '.', '.'}, {'.', '.', '.'}, {'.', '.', '.'}};

    /********************
        MAIN GAME LOOP
     *******************/
    while (gameover == 0) {
        //read the current player's move, starting with x
        bzero(buff, buff_MAX);
        if (current_player == 0) {
            read(game->playerx, buff, sizeof(buff));
        } else {
            read(game->playero, buff, sizeof(buff));
        }
        //check if the data is correct
        if (buff[0] == 'M' && buff[1] == 'O' && buff[2] == 'V' && buff[3] == 'E' && buff[4] == '|') {
            if (buff[5] != 6 || buff[6] != '|') {
                printf("Invalid command\n");
                continue;
            }
            if (buff[7] == 'X' && current_player == 1) {
                printf("Invalid command\n");
                continue;
            } else if (buff[7] == 'O' && current_player == 0) {
                printf("Invalid command\n");
                continue;
            }

            int x = buff[9] - '0';
            int y = buff[11] - '0';
            if (x < 1 || x > 3 || y < 1 || y > 3) {
                printf("Invalid command\n");
                continue;
            }
            if (board[x - 1][y - 1] != '.') {
                printf("Invalid command\n");
                continue;
            }
            //MOVE|6|X|1,1|
            //if the move is valid, update the board
            if (current_player == 0) {
                board[x - 1][y - 1] = 'X';
            } else {
                board[x - 1][y - 1] = 'O';
            }
            current_player = (current_player + 1) % 2;
            //check if the game is over
            gameover = isGameOver(board);

            //send the move to the other player

        } else  {
            //return an error

            printf("Invalid command\n");
        }





    }


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
            clientO_username[usernameLength] = '\0';

            flag = 1;
            printf("Client O added to game: %s\n", username);
        } else {
            clientX_placeholder = clientO;
            strcpy(clientX_username, username);
            clientX_username[usernameLength] = '\0';
            flag = 0;
            printf("Client X added to game: %s\n", username);
            //create a thread to handle the game
            game = malloc(sizeof(Game));
            game->playero = clientO_placeholder;
            game->playerx = clientX_placeholder;
            strcpy(game->usernameo, clientO_username);
            strcpy(game->usernamex, clientX_username);

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