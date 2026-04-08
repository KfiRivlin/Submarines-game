#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define SIZE 6
#define NUM_SEGMENTS 6

int instructions_and_difficulty();
void game(int difficulty);
void print_map(char friendly[SIZE+1][SIZE+1], char enemy[SIZE+1][SIZE+1]);
void print_line(int row,char friendly[SIZE+1][SIZE+1], char enemy[SIZE+1][SIZE+1]);
void zero_matrix(char matrix[SIZE+1][SIZE+1]);
void load_matrices(char friendly[SIZE+1][SIZE+1], char enemy[SIZE+1][SIZE+1],int *f_board,int *e_board);
void check_sunk_friendly(char friendly[SIZE+1][SIZE+1], int f_board);
void check_sunk_enemy(char enemy[SIZE+1][SIZE+1], int e_board);
void friendly_turn(char enemy[SIZE+1][SIZE+1], int e_board, int *f_counter);
void enemy_turn(char friendly[SIZE+1][SIZE+1], int f_board, int *e_counter, int last_shot[3], int difficulty);
void calc_enemy_shot(char friendly[SIZE+1][SIZE+1], int last_shot[3], int *row, int *col, int difficulty);
bool out_of_bounds(int row, int col);
bool shot_legal(char friendly[SIZE+1][SIZE+1], int row, int col);
bool lucky_shot(char friendly[SIZE+1][SIZE+1], int difficulty, int *row, int *col);
bool lucky_hit(char friendly[SIZE+1][SIZE+1], int row, int col);

int main() {
    int difficulty = instructions_and_difficulty();
    game(difficulty);
    return 0;
}

int instructions_and_difficulty() {
    printf("\nGame instructions:\n"
        "* You are playing against the computer, the submarines will be placed for you.\n"
        "Do not worry, the game is not rigged (it may be rigged ;).\n"
        "* The game will operates on a text based interface. each round you will place"
        " your next shot by typing the row (up-down) and column (left-right)\n"
        "* Your map is on the left , the enemy map on is the right\n"
        "* Each player has 2 vessels, both 3 segments long\n"
        "* Guide for shot marking:\n    0 for missed shot\n    X for a hit\n    $ for sunken vessel"
        "\n    F for your vessels\n"
        "* The first to sink all enemy submarines will win nothing\n\nGood luck!\n\n"
        "Choose Difficulty:\n   1. Easy\n   2. Challenging\n   3. Hard\n   4. Extreme\n\n"
        "Your choice: ");
    int difficulty;
    scanf(" %d",&difficulty);
    while (difficulty < 1 || difficulty > 4) {
        printf("Choose Difficulty:\n   1. Easy\n   2. Medium\n   3. Hard\n   4. Extreme\n\n"
        "Your choice: ");
        scanf(" %d",&difficulty);
    }
    return difficulty;
}

void game(int difficulty) {
    char friendly[SIZE+1][SIZE+1], enemy[SIZE+1][SIZE+1];
    int friendly_board, enemy_board, friendly_counter = 0, enemy_counter = 0;
    zero_matrix(friendly);
    zero_matrix(enemy);
    load_matrices(friendly,enemy,&friendly_board,&enemy_board);
    int enemy_last_shot[3] = {0};  //[row , column , flag (1 for hit/0 for miss)]
    while (friendly_counter < NUM_SEGMENTS && enemy_counter < NUM_SEGMENTS) {
        print_map(friendly,enemy);
        friendly_turn(enemy,enemy_board,&friendly_counter);
        if (friendly_counter == NUM_SEGMENTS) break;
        enemy_turn(friendly,friendly_board,&enemy_counter,enemy_last_shot,difficulty);
    }
    print_map(friendly,enemy);
    if (friendly_counter == NUM_SEGMENTS) {
        printf("\n\nAll enemy vessels are under!\nVictory! good game.");
        return;
    }
    printf("\n\nYour whole navy was sunk... Better luck next time.");
}

void print_map(char friendly[SIZE+1][SIZE+1], char enemy[SIZE+1][SIZE+1]) {
    char buffer[] = "+---+---+---+---+---+---+---+";
    printf("%+18s  %+35s\n","FRIENDLY","ENEMY");
    printf("|   ");
    for (int i = 1; i <= SIZE; i++) {
        printf("| %-2d",i);
    }
    printf("|         |   ");
    for (int i = 1; i <= SIZE; i++) {
        printf("| %-2d",i);
    }
    printf("|\n%s         %s\n",buffer,buffer);
    for (int i = 1; i <= SIZE; i++) {
        print_line(i, friendly, enemy);
        printf("%s         %s\n",buffer,buffer);
    }
}

void print_line(int row,char friendly[SIZE+1][SIZE+1], char enemy[SIZE+1][SIZE+1]) {
    printf("| %d ",row);
    for (int i = 1; i <= SIZE; i++) {       //friendly
        printf("| %-2c",friendly[row][i]);
    }
    printf("|         | %d ",row);
    for (int i = 1; i <= SIZE; i++) {       //enemy
        char c = enemy[row][i];
        if (c == 'E') {
            printf("| %-2c",' ');
        } else {
            printf("| %-2c",c);
        }
    }

    printf("|\n");
}

void zero_matrix(char matrix[SIZE+1][SIZE+1]) {
    for (int i = 0; i <= SIZE; i++) {
        for (int j = 0; j <= SIZE; j++) {
            matrix[i][j] = ' ';
        }
    }
}

void load_matrices(char friendly[SIZE+1][SIZE+1], char enemy[SIZE+1][SIZE+1],int *f_board,int *e_board) {
    srand(time(NULL));
    *f_board = rand()%4;
    if (*f_board == 0) {
        for (int i = 0; i < 3; i++) friendly[2][i+1] = 'F';
        for (int i = 0; i < 3; i++) friendly[i+4][4] = 'F';
    }
    if (*f_board == 1) {
        for (int i = 0; i < 3; i++) friendly[1][i+2] = 'F';
        for (int i = 0; i < 3; i++) friendly[i+1][6] = 'F';
    }
    if (*f_board == 2) {
        for (int i = 0; i < 3; i++) friendly[i+3][1] = 'F';
        for (int i = 0; i < 3; i++) friendly[i+1][5] = 'F';
    }
    if (*f_board == 3) {
        for (int i = 0; i < 3; i++) friendly[3][i+1] = 'F';
        for (int i = 0; i < 3; i++) friendly[1][i+4] = 'F';
    }
    *e_board = rand()%4;
    if (*e_board == 0) {
        for (int i = 0; i < 3; i++) enemy[3][i+1] = 'E';
        for (int i = 0; i < 3; i++) enemy[i+4][5] = 'E';
    }
    if (*e_board == 1) {
        for (int i = 0; i < 3; i++) enemy[1][i+1] = 'E';
        for (int i = 0; i < 3; i++) enemy[i+3][6] = 'E';
    }
    if (*e_board == 2) {
        for (int i = 0; i < 3; i++) enemy[i+1][2] = 'E';
        for (int i = 0; i < 3; i++) enemy[i+2][5] = 'E';
    }
    if (*e_board == 3) {
        for (int i = 0; i < 3; i++) enemy[5][i+1] = 'E';
        for (int i = 0; i < 3; i++) enemy[1][i+3] = 'E';
    }
}

void check_sunk_friendly(char friendly[SIZE+1][SIZE+1], int f_board) {
    if (f_board == 0) {
        if (friendly[2][1] == 'X' && friendly[2][2] == 'X' && friendly[2][3] == 'X') {
            friendly[2][1] = '$';
            friendly[2][2] = '$';
            friendly[2][3] = '$';
        }
        if (friendly[4][4] == 'X' && friendly[5][4] == 'X' && friendly[6][4] == 'X') {
            friendly[4][4] = '$';
            friendly[5][4] = '$';
            friendly[6][4] = '$';
        }
    }
    else if (f_board == 1) {
        if (friendly[1][2] == 'X' && friendly[1][3] == 'X' && friendly[1][4] == 'X') {
            friendly[1][2] = '$';
            friendly[1][3] = '$';
            friendly[1][4] = '$';
        }
        if (friendly[1][6] == 'X' && friendly[2][6] == 'X' && friendly[3][6] == 'X') {
            friendly[1][6] = '$';
            friendly[2][6] = '$';
            friendly[3][6] = '$';
        }
    }
    else if (f_board == 2) {
        if (friendly[3][1] == 'X' && friendly[4][1] == 'X' && friendly[5][1] == 'X') {
            friendly[3][1] = '$';
            friendly[4][1] = '$';
            friendly[5][1] = '$';
        }
        if (friendly[1][5] == 'X' && friendly[2][5] == 'X' && friendly[3][5] == 'X') {
            friendly[1][5] = '$';
            friendly[2][5] = '$';
            friendly[3][5] = '$';
        }
    }
    else if (f_board == 3) {
        if (friendly[3][1] == 'X' && friendly[3][2] == 'X' && friendly[3][3] == 'X') {
            friendly[3][1] = '$';
            friendly[3][2] = '$';
            friendly[3][3] = '$';
        }
        if (friendly[1][4] == 'X' && friendly[1][5] == 'X' && friendly[1][6] == 'X') {
            friendly[1][4] = '$';
            friendly[1][5] = '$';
            friendly[1][6] = '$';
        }
    }
}

void check_sunk_enemy(char enemy[SIZE+1][SIZE+1], int e_board) {
    if (e_board == 0) {
        if (enemy[3][1] == 'X' && enemy[3][2] == 'X' && enemy[3][3] == 'X') {
            enemy[3][1] = '$';
            enemy[3][2] = '$';
            enemy[3][3] = '$';
        }
        if (enemy[4][5] == 'X' && enemy[5][5] == 'X' && enemy[6][5] == 'X') {
            enemy[4][5] = '$';
            enemy[5][5] = '$';
            enemy[6][5] = '$';
        }
    }
    else if (e_board == 1) {
        if (enemy[1][1] == 'X' && enemy[1][2] == 'X' && enemy[1][3] == 'X') {
            enemy[1][1] = '$';
            enemy[1][2] = '$';
            enemy[1][3] = '$';
        }
        if (enemy[3][6] == 'X' && enemy[4][6] == 'X' && enemy[5][6] == 'X') {
            enemy[3][6] = '$';
            enemy[4][6] = '$';
            enemy[5][6] = '$';
        }
    }
    else if (e_board == 2) {
        if (enemy[1][2] == 'X' && enemy[2][2] == 'X' && enemy[3][2] == 'X') {
            enemy[1][2] = '$';
            enemy[2][2] = '$';
            enemy[3][2] = '$';
        }
        if (enemy[2][5] == 'X' && enemy[3][5] == 'X' && enemy[4][5] == 'X') {
            enemy[2][5] = '$';
            enemy[3][5] = '$';
            enemy[4][5] = '$';
        }
    }
    else if (e_board == 3) {
        if (enemy[5][1] == 'X' && enemy[5][2] == 'X' && enemy[5][3] == 'X') {
            enemy[5][1] = '$';
            enemy[5][2] = '$';
            enemy[5][3] = '$';
        }
        if (enemy[1][3] == 'X' && enemy[1][4] == 'X' && enemy[1][5] == 'X') {
            enemy[1][3] = '$';
            enemy[1][4] = '$';
            enemy[1][5] = '$';
        }
    }
}

void friendly_turn(char enemy[SIZE+1][SIZE+1], int e_board, int *f_counter) {
    int row, col;
    printf("\nPick your next shot!\nEnter row: ");
    scanf("%d",&row);
    printf("Enter column: ");
    scanf("%d",&col);
    while (out_of_bounds(row,col) || !shot_legal(enemy,row,col)) {
        printf("\nShot placement error.\nPick your next shot!\nEnter row: ");
        scanf("%d",&row);
        printf("Enter column: ");
        scanf("%d",&col);
    }
    if (enemy[row][col] == ' ') enemy[row][col] = '0';
    if (enemy[row][col] == 'E') {
        enemy[row][col] = 'X';
        (*f_counter)++;
        check_sunk_enemy(enemy,e_board);
    }
}

void enemy_turn(char friendly[SIZE+1][SIZE+1], int f_board, int *e_counter, int last_shot[3], int difficulty) {
    int row;
    int col;
    calc_enemy_shot(friendly,last_shot,&row,&col,difficulty);
    last_shot[0] = row;
    last_shot[1] = col;
    if (friendly[row][col] == ' ') {
        friendly[row][col] = '0';
        last_shot[2] = 0;
    }
    if (friendly[row][col] == 'F') {
        friendly[row][col] = 'X';
        last_shot[2] = 1;
        (*e_counter)++;
        check_sunk_friendly(friendly,f_board);
    }
}

void calc_enemy_shot(char friendly[SIZE+1][SIZE+1], int last_shot[3], int *row, int *col, int difficulty) {
    int count = 0;
    if (last_shot[2] == 1) {
        do {
            count++;
            int direction = rand()%4;
            *row = last_shot[0];
            *col = last_shot[1];
            switch (direction) {
                case 0: (*row)++; break;
                case 1: (*row)--; break;
                case 2: (*col)++; break;
                case 3: (*col)--; break;
            }
        } while ((out_of_bounds(*row,*col) || !shot_legal(friendly,*row,*col)) && count < 8);
        if (count < 8) return;
    }
    if (lucky_shot(friendly,difficulty,row,col)) return;
    if (last_shot[2] == 0 || count == 8) {
        do {
            *row = rand()%SIZE + 1;
            *col = rand()%SIZE + 1;
        } while (!shot_legal(friendly,*row,*col));
    }
}

bool out_of_bounds(int row, int col) {
    if (row < 1 || row > SIZE || col < 1 || col > SIZE) {
        return true;
    }
    return false;
}

bool shot_legal(char board[SIZE+1][SIZE+1], int row, int col) {
    if (board[row][col] == '0' || board[row][col] == 'X' || board[row][col] == '$') {
        return false;
    }
    return true;
}

bool lucky_shot(char friendly[SIZE+1][SIZE+1], int difficulty, int *row, int *col) {
    if (difficulty == 1) return false;
    int num_tries;
    switch (difficulty) {
        case 2: num_tries = 2; break;
        case 3: num_tries = 8; break;
        case 4: num_tries = 20; break;
    }
    for (int i = 0; i < num_tries; i++) {
        *row = rand()%SIZE + 1;
        *col = rand()%SIZE + 1;
        if (lucky_hit(friendly,*row,*col)) return true;
    }
    return false;
}

bool lucky_hit(char friendly[SIZE+1][SIZE+1], int row, int col) {
    if (friendly[row][col] == 'F') return true;
    return false;
}