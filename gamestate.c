#include "gamestate.h"

void nullGame(GameState* game) {
    *game = (GameState){false, 0, -1, -1, -1, -1, NULL};
}

void printGame(GameState* game) {
    printf("\nCols:%d \nRows:%d \nbombsNum:%d \nbombsPer:%f \n", game->columns, game->rows, game->bombsNum, game->bombsPer);
}
