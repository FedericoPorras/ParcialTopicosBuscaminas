#include "gamestate.h"

void nullGame(GameState* game) {
    game->bombsNum = -1;
    game->field = NULL;
    game->logFile = NULL;
    game->rows = -1;
    game->columns = -1;
    game->seed = -1;
    game->started = false;
    game->lost[0] = -1;
    game->lost[1] = -1;
    // TODO game->lost;
}

void printGame(GameState* game) {
    printf("\nCols:%d \nRows:%d \nbombsNum:%d \n", game->columns, game->rows, game->bombsNum);
}

void saveGame(GameState* game, FILE* file) {
    int headers[6] = {game->seed, game->columns, game->rows, game->bombsNum, game->lost[0], game->lost[1]};

    fseek(file, 0, SEEK_SET);
    fwrite(&game->started, sizeof(bool), 1, file);
    fwrite(&headers, sizeof(int), 6, file);

    for (int i=0; i<game->rows; i++) {
        for (int j=0; j<game->columns; j++) {
            fwrite(&game->field[i][j], sizeof(mineCeld), 1, file);
        }
    }
}

void loadGame(GameState* game, FILE* file) {
    fread(&game->started, sizeof(bool), 1, file);
    int headers[6];
    fread(headers, sizeof(int), 6, file);

    game->seed = headers[0];
    game->columns = headers[1];
    game->rows = headers[2];
    game->bombsNum = headers[3];
    game->lost[0] = headers[4];
    game->lost[1] = headers[5];

    game->field = (mineCeld**)newDinMtx(game->rows, game->columns, sizeof(mineCeld));

    for (int i=0; i<game->rows; i++) {
        for (int j=0; j<game->columns; j++) {
            fread(&game->field[i][j], sizeof(mineCeld), 1, file);
        }
    }

    game->logFile = NULL; // TODO: Maybe save the log path and continue writing it
    game->binFile = file;
}
