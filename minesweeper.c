#include "minesweeper.h"

//#include <windows.h>



// This consts will be used in some recursive functions, for saving memory they are declared with global scope
const int di[] = {-1,-1,-1, 0, 0, 1, 1, 1};
const int dj[] = {-1, 0, 1,-1, 1,-1, 0, 1};
const char masks[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};



void emptyField(int rows, int cols, mineCeld** field) {
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            field[i][j] = (mineCeld){false, false, false, 0};
        }
    }
}

int randomBombs(int rows, int cols, mineCeld** field, int ammount, int seed, int pos[2]) {
    // Load a seed
    int thisSeed;

    if (seed == -1) {
        thisSeed = time(NULL);
        srand(thisSeed);
    }
    else srand(seed); // or random it

    int i=0;
    int randomNum1, randomNum2;
    while(i<ammount) {
        randomNum1 = rand();
        randomNum2 = rand();
        mineCeld* random = &(field[randomNum1%rows][randomNum2%cols]);
        if (!random->bomb && random != &field[*pos][*(pos+1)]) {
            random->bomb = true;
            i++;
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
    char rv = 0x0;

    for (int k=0; k<8; k++) {
        int ni = i+di[k];
        int nj = j+dj[k];
        if (ni >= 0 && ni < rows && nj >= 0 && nj < cols)
            rv |= masks[k];
    }

    return rv;
}

void calcAdjacency(int rows, int cols, mineCeld** field) {
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            if (field[i][j].bomb) field[i][j].adjacency = -1;
            else {
                char where_search = check_position(i, j, rows, cols);
                int adj=0;
                for (int i2=0; i2<8; i2++)
                    if (where_search & masks[i2] && field[i + di[i2]][j + dj[i2]].bomb)
                        adj++;
                field[i][j].adjacency = adj;
            }
        }
    }
}

bool checkNoAdjacencies(int i, int j, int rows, int cols, mineCeld** field) {

    if (field[i][j].bomb || field[i][j].flag) return false;

    char where_search = check_position(i, j, rows, cols);

    for (int i2=0; i2<8; i2++) {
        if (where_search & masks[i2] && field[i + di[i2]][j + dj[i2]].bomb && !field[i + di[i2]][j + dj[i2]].flag)
            return false;
    }

    return true;
}

void revealAdjacencies(int i, int j , int rows, int cols, mineCeld** field) {
    if (checkNoAdjacencies(i,j,rows,cols,field)) field[i][j].revealed = true;
    else {
        if (!field[i][j].bomb && !field[i][j].flag) field[i][j].revealed = true;
        return;
    }

    char cp = check_position(i, j, rows, cols);
    for (int i2=0; i2<8; i2++)
        if (cp & masks[i2] && !field[i + di[i2]][j + dj[i2]].revealed)
            revealAdjacencies(i + di[i2], j + dj[i2], rows, cols, field);
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
            if (
                !field[i][j].revealed &&
                (!field[i][j].bomb || (field[i][j].bomb && !field[i][j].flag))
            )
                return false;
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
            printf("|");
        }
        printf("\n");
    }
}

void initField(GameState* game, int pos[2]) {
    emptyField(game->rows, game->columns, game->field);
    game->seed = randomBombs(game->rows, game->columns, game->field, game->bombsNum, game->seed, pos);
    calcAdjacency(game->rows, game->columns, game->field);
    game->started = true;
}

int userRevealCeld(int i, int j, GameState* game) {
    if (!game->field[i][j].bomb) {
        game->field[i][j].revealed = true;
        revealAdjacencies(i, j, game->rows, game->columns, game->field);
        return CELD_EMPTY;
    } else if (!game->field[i][j].flag) {
        revealAll(game->rows, game->columns, game->field);
        game->lost[0] = i; game->lost[1] = j;
        return CELD_BOMB;
    } else return CELD_FLAGGED;
}

void userAuxActionCeld(int i, int j, GameState* game) {
    if (game->field[i][j].flag) game->field[i][j].flag = false;
    else game->field[i][j].flag = true;
}

void logFileWriteClick(char button, int* posInMesh, FILE* file) {
    char date[20];
    time_t t = time(NULL);
    struct tm* now = localtime(&t);
    sprintf(date, "%04d-%02d-%02d-%02d-%02d-%02d", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);

    fprintf(file, "\nEvent:Click/Type:%c/PosMesh:(%02d,%02d)/Date:(%s)", button, *posInMesh, *(posInMesh+1), date);
}

int minesUncovered(int bombsNum, int rows, int cols, mineCeld** field) {
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            if (field[i][j].flag)
                bombsNum--;
        }
    }
    return bombsNum;
}


