#include "gamestate.h"

void nullGame(GameState* game) {
    game->bombsNum = -1;
    game->field = NULL;
    game->logFile = NULL;
    game->rows = -1;
    game->columns = -1;
    game->seed = -1;
    game->started = false;
    // TODO game->lost;
}

/*
    bool started;
    int seed;
    int columns;
    int rows;
    int bombsNum;
    float bombsPer; // percentage
    int lost[2];
    mineCeld** field;
    FILE* logFile;
*/

void printGame(GameState* game) {
    printf("\nCols:%d \nRows:%d \nbombsNum:%d \n", game->columns, game->rows, game->bombsNum);
}
