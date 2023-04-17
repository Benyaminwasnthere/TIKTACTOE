
#include <stdio.h>


void checkResult(char board[3][3]) {
    int flag = 1;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == '.') {
                flag = 0;
                break;
            }
        }
    }
    if (flag == 0) {
        if (checkWinner('o', board) == 1 && checkWinner('x', board) == 0) {
            printf("o");

        }else if (checkWinner('o', board) == 0 && checkWinner('x', board) == 1) {
            printf("x");

        }
        else if (checkWinner('o', board) == 1 && checkWinner('x', board) == 1) {
            printf("both -draw");


        }
        else if (checkWinner('o', board) == 0 && checkWinner('x', board) == 0) {
            printf("none-draw");

        }
        else {
            printf("game isnt finished");
            return NULL;
        }
    }
    if (flag == 1) {
        if (checkWinner('o', board) == 1 && checkWinner('x', board) == 0) {
            printf("o");

        }
        else if (checkWinner('o', board) == 0 && checkWinner('x', board) == 1) {
            printf("x");

        }
        else if (checkWinner('o', board) == 1 && checkWinner('x', board) == 1) {
            printf("both -draw");


        }
        else if (checkWinner('o', board) == 0 && checkWinner('x', board) == 0) {
            printf("none-draw");

        }
        else {
            printf("game has finished draw");
            return NULL;
        }
       
    }
    
    
    
}

int checkWinner(char sg,char board[3][3])
{
    // check all rows
    if (board[0][0] == sg && board[0][1] == sg && board[0][2] == sg ||
        board[1][0] == sg && board[1][1] == sg && board[1][2] == sg ||
        board[2][0] == sg && board[2][1] == sg && board[2][2] == sg)
    {
       
        return 1;
    }
    // check all columns
    else if (board[0][0] == sg && board[1][0] == sg && board[2][0] == sg ||
        board[0][1] == sg && board[1][1] == sg && board[2][1] == sg ||
        board[0][2] == sg && board[1][2] == sg && board[2][2] == sg)
    {
        
        return 1;
    }
    // check both diagonals
    else if (board[0][0] == sg && board[1][1] == sg && board[2][2] == sg ||
        board[0][2] == sg && board[1][1] == sg && board[2][0] == sg)
    {
       
        return 1;
       
    }
    
    // There is no winner
    return 0;
  
}





int main (){
    char board[3][3] = {
    {'x','o' , 'o'},
    {'x', 'x', 'o'},
    {'x', '.', 'o'}
    };
    checkResult(board);
    return 0;
}
