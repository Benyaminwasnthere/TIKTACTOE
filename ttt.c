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

    player = buff[7];
    printf("You are player %c\n", player);

    char player_turn = 'X';

    //make the main game loop
    while (1) {
        if (player == player_turn) {
            printf("It is your turn\n");
            printf("Enter your move: ");
            fgets(buff, MAX_BUFFER, stdin);
            buff[strlen(buff) - 1] = '\0';

            //check if the move is "RSGN"
            if (strncmp(buff, "RSGN", 4) == 0) {
                strcat(buff, "|0|");
                printf("Sending command: %s\n", buff);
                write(sock, buff, sizeof(buff));

                bzero(buff, sizeof(buff));
                read(sock, buff, 4);
                printf("Received: %s\n", buff);

                if (strncmp(buff, "OVER", 4) == 0) {
                    read(sock, buff, MAX_BUFFER);
                    if (buff[4] == 'W') {
                        printf("You won!\n");
                    } else if (buff[4] == 'L') {
                        printf("You lost!\n");
                    }
                    int i = 6;
                    while (buff[i] != '|') {
                        if (buff[i] == '\n' || buff[i] == '\0') {
                            i++;
                            continue;
                        }
                        printf("%c", buff[i]);
                        i++;
                    }
                    printf("\n");
                }

                close(sock);
                return 0;

            }

            bzero(command, sizeof(command));
            strcpy(command, "MOVE|6|");
            strcat(command, &player);
            strcat(command, "|");
            strcat(command, buff);
            strcat(command, "|");
            printf("Sending command: %s\n", command);
            write(sock, command, sizeof(command));

            bzero(buff, sizeof(buff));
            read(sock, buff, 4);
            printf("Received: %s\n", buff);

            if (strncmp(buff, "INVL", 4) == 0) {
                printf("Invalid move....\n");
                read(sock, buff, MAX_BUFFER);
                continue;
            }
            if (strncmp(buff, "MOVD", 4) == 0) {
                printf("Move successful\n");
            }
            if (strncmp(buff, "OVER", 4) == 0) {

                read(sock, buff, MAX_BUFFER);
                if (buff[4] == 'W') {
                    printf("You won!\n");
                } else if (buff[4] == 'L') {
                    printf("You lost!\n");
                }
                int i = 6;
                while (buff[i] != '|') {
                    if (buff[i] == '\n' || buff[i] == '\0') {
                        i++;
                        continue;
                    }
                    printf("%c", buff[i]);
                    i++;
                }
                printf("\n");

                //end the connection
                close(sock);
                return 0;

            }

            bzero(buff, sizeof(buff));
            read(sock, buff, MAX_BUFFER);
            //print the board
            int i = 10;
            printf("The board is now:\n");
            for (int j = 0; j < 3; j++) {
                printf("%d ", j);
                for (int k = 0; k < 3; k++) {
                    printf("%c ", buff[i]);
                    i++;
                }
                printf("\n");
            }

            if (player_turn == 'X') {
                player_turn = 'O';
            } else {
                player_turn = 'X';
            }
        } else {
            printf("It is your opponent's turn\n");
            bzero(buff, sizeof(buff));
            read(sock, buff, 4);
            if (strncmp(buff, "MOVD", 4) != 0 && strncmp(buff, "OVER", 4) != 0) {
                printf("Invalid data received (NOT MOVD)\n");
                continue;
            }
            if (strncmp(buff, "OVER", 4) == 0) {
                bzero(buff, sizeof(buff));
                read(sock, buff, MAX_BUFFER);
                if (buff[4] == 'W') {
                    printf("You won!\n");
                } else if (buff[4] == 'L') {
                    printf("You lost!\n");
                }
                int i = 6;
                while (buff[i] != '|') {
                    if (buff[i] == '\n' || buff[i] == '\0') {
                        i++;
                        continue;
                    }
                    printf("%c", buff[i]);
                    i++;
                }
                printf("\n");

                //end the connection
                close(sock);
                return 0;
            }

            bzero(buff, sizeof(buff));
            read(sock, buff, MAX_BUFFER);

            int i = 10;
            //print the board
            printf("Your opponent played:\n");
            for (int j = 0; j < 3; j++) {
                printf("%d ", j);
                for (int k = 0; k < 3; k++) {
                    printf("%c ", buff[i]);
                    i++;
                }
                printf("\n");
            }

            if (player_turn == 'X') {
                player_turn = 'O';
            } else {
                player_turn = 'X';
            }


        }

    }



}
