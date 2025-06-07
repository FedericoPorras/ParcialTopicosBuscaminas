#include "minesweeper.h"

void emptyField(int rows, int cols, mineCeld** field) {
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            field[i][j] = (mineCeld){false, false, false, false, 0};
        }
    }
}

int randomBombs(int rows, int cols, mineCeld** field, int ammount, int seed, int pos[2]) {
    // Load a seed
    int thisSeed;
    bool correctGen=false;

    while (!correctGen) { // TODO: Check how much time costs this loop because sometimes here the execution stops
        if (seed == -1) {
            thisSeed = time(NULL);
            srand(thisSeed);
        }
        else srand(seed); // or random it

        // Put the bombs
        int i=0;
        while(i<ammount) {
            mineCeld* random = &(field[rand()%rows][rand()%cols]);
            if (!random->bomb) {
                random->bomb = true;
                i++;
            }
        }

        if (!field[*pos][*(pos+1)].bomb) {
            correctGen = true;
        }
    }

    if (seed == -1) return thisSeed;
    else return seed;

}

char check_position(int i, int j, int rows, int cols) {
    // bits
    // 012
    // 3X4
    // 567
    const int di[] = {-1,-1,-1, 0, 0, 1, 1, 1};
    const int dj[] = {-1, 0, 1,-1, 1,-1, 0, 1};
    const char masks[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
    char rv = 0x0;

    for (int k=0; k<8; k++) {
        int ni = i+di[k];
        int nj = j+dj[k];
        if (ni >= 0 && ni < rows && nj >= 0 && nj < cols)
            rv |= masks[k];
    }

    return rv;
}

void calcAdjacency(int rows, int cols, mineCeld** field) {// TODO: USE HEXA INSTEAD OF BINARY! USE MASKS[]!
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            if (field[i][j].bomb) field[i][j].adjacency = -1;
            else {
                char where_search = check_position(i, j, rows, cols);
                int adj=0;
                if (where_search & 0b00000001 && field[i-1][j-1].bomb) adj++;
                if (where_search & 0b00000010 && field[i-1][j]  .bomb) adj++;
                if (where_search & 0b00000100 && field[i-1][j+1].bomb) adj++;
                if (where_search & 0b00001000 && field[i]  [j-1].bomb) adj++;
                if (where_search & 0b00010000 && field[i]  [j+1].bomb) adj++;
                if (where_search & 0b00100000 && field[i+1][j-1].bomb) adj++;
                if (where_search & 0b01000000 && field[i+1][j]  .bomb) adj++;
                if (where_search & 0b10000000 && field[i+1][j+1].bomb) adj++;
                field[i][j].adjacency = adj;

            }
        }
    }
}

bool checkNoAdjacencies(int i, int j, int rows, int cols, mineCeld** field) {// TOO SIMILAR TO CALCADJACENCY! maybe put it together
            char where_search = check_position(i, j, rows, cols);
            int adj=0;
            if (where_search & 0b00000001 && field[i-1][j-1].bomb && !field[i-1][j-1].flag) adj++;
            if (where_search & 0b00000010 && field[i-1][j]  .bomb && !field[i-1][j]  .flag) adj++;
            if (where_search & 0b00000100 && field[i-1][j+1].bomb && !field[i-1][j+1].flag) adj++;
            if (where_search & 0b00001000 && field[i]  [j-1].bomb && !field[i]  [j-1].flag) adj++;
            if (where_search & 0b00010000 && field[i]  [j+1].bomb && !field[i]  [j+1].flag) adj++;
            if (where_search & 0b00100000 && field[i+1][j-1].bomb && !field[i+1][j-1].flag) adj++;
            if (where_search & 0b01000000 && field[i+1][j]  .bomb && !field[i+1][j]  .flag) adj++;
            if (where_search & 0b10000000 && field[i+1][j+1].bomb && !field[i+1][j+1].flag) adj++;

            if (!adj && !field[i][j].bomb  && !field[i][j].flag) return true; // delete '!field[i][j].bomb' condition
            else return false;
}

void revealAdjacencies(int i, int j , int rows, int cols, mineCeld** field) { // TODO: Put better this kind of things. USE MASKS[] and the other array!
    if (checkNoAdjacencies(i,j,rows,cols,field)) field[i][j].revealed = true;
    else {
        if (!field[i][j].bomb && !field[i][j].flag) field[i][j].revealed = true;
        return;
    }

    char cp = check_position(i, j, rows, cols);
    if (cp & 0b00000001 && !field[i-1][j-1].revealed) revealAdjacencies(i-1, j-1, rows, cols, field);
    if (cp & 0b00000010 && !field[i-1][j]  .revealed) revealAdjacencies(i-1, j  , rows, cols, field);
    if (cp & 0b00000100 && !field[i-1][j+1].revealed) revealAdjacencies(i-1, j+1, rows, cols, field);
    if (cp & 0b00001000 && !field[i]  [j-1].revealed) revealAdjacencies(i  , j-1, rows, cols, field);
    if (cp & 0b00010000 && !field[i]  [j+1].revealed) revealAdjacencies(i  , j+1, rows, cols, field);
    if (cp & 0b00100000 && !field[i+1][j-1].revealed) revealAdjacencies(i+1, j-1, rows, cols, field);
    if (cp & 0b01000000 && !field[i+1][j]  .revealed) revealAdjacencies(i+1, j  , rows, cols, field);
    if (cp & 0b10000000 && !field[i+1][j+1].revealed) revealAdjacencies(i+1, j+1, rows, cols, field);
}

void revealAll(int rows, int cols, mineCeld** field) {
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            field[i][j].revealed = true;
        }
    }
}

void coverAll(int rows, int cols, mineCeld** field) {
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            field[i][j].revealed = false;
            field[i][j].flag = false;
        }
    }
}

bool checkWin(int rows, int cols, mineCeld** field) {
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            if ((!field[i][j].revealed && !(field[i][j].flag && field[i][j].bomb)) || field[i][j].loose) return false; // TODO: CHANGE, loose will be analized in other function
        }
    }
    return true;
}

void printFieldConsole(int rows, int cols, mineCeld** field) {
    for (int i=0; i<rows; i++) {
        printf("|");
        for (int j=0; j<cols; j++) {
            if (field[i][j].bomb)
                printf("X");
            else
                printf("%d", field[i][j].adjacency);
            //printf(field[i][j].bomb ? "X" : " ");
            printf("|");
        }
        printf("\n");
    }
}

