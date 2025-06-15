#include "gamestate.h"

void nullGame(GameState* game) {
    game->bombsNum = -1;
    game->binFile = NULL;
    game->logFile = NULL;
    game->rows = -1;
    game->columns = -1;
    game->seed = -1;
    game->started = false;
    game->lost[0] = -1;
    game->lost[1] = -1;
    game->timeStart = 0;
    game->timeFinish = 0;
    strcpy(game->nameUser, "<Anonymous user>");
}

void printGame(GameState* game) {
    printf("\nStarted: %d\nSeed: %d \nCols:%d \nRows:%d \nbombsNum:%d \n", (int)game->started, game->seed, game->columns, game->rows, game->bombsNum);

}

void saveGameBin(GameState* game, FILE* file) { // TODO: Save the struct game instead of each field
    int headers[8] = {game->seed, game->columns, game->rows, game->bombsNum, game->lost[0], game->lost[1], game->last_selected[0], game->last_selected[1]};

    fseek(file, 0, SEEK_SET);
    fwrite(&game->started, sizeof(bool), 1, file);
    fwrite(&headers, sizeof(int), 8, file);

    for (int i=0; i<game->rows; i++) {
        for (int j=0; j<game->columns; j++) {
            fwrite(&game->field[i][j], sizeof(mineCeld), 1, file);
        }
    }

    fwrite(&game->timeStart, sizeof(size_t), 1, file);
    fwrite(&game->timeFinish, sizeof(size_t), 1, file);

    fclose(game->binFile);
    game->binFile = NULL;
}

struct tm timeGame(time_t timeG) {
    time_t now;
    time(&now);
    time_t dif = (time_t)difftime(now, timeG);
    return *gmtime(&dif);
}

struct tm timeEndGame(GameState* game) {
    time_t dif = (time_t)difftime(game->timeFinish, game->timeStart);
    struct tm dif2 = *gmtime(&dif);
    return dif2;
}

void saveGameStatsLast10(GameState* game, char* namePlayer, int gameEnd) { // TODO: PUT BETTER THIS file must be open only once / in r+t. Delete debugging prints

    FILE* last10file = fopen("last10.txt", "ab");
    if (!last10file) {
        last10file = fopen("last10.txt", "wb");
        if (!last10file) {printf("\nError opening last10 file."); return;}
    }


    if (namePlayer[0] == '\0' || strchr(namePlayer, '/'))
        strcpy(namePlayer, "Invalid Name");

    char gameEndWrite[5];
    switch (gameEnd) {
        case 1: strcpy(gameEndWrite, "Won"); break;
        case 2: strcpy(gameEndWrite, "Lost"); break;
        case 3: strcpy(gameEndWrite, "    "); break;
    }

    int stats[3];
    gameStats(game, stats);


    struct tm timePlayed;
    if (game->timeFinish == 0) timePlayed = timeGame(game->timeStart); // TODO here diff days/years
    else timePlayed = timeEndGame(game);


    char toWrite[LEN_SAVE10LOG+1];
    sprintf(toWrite, "Name:%30s/Dimension:%02dx%02d/Mines_pending:%04d/Mines_discovered:%04d/Flags:%04d/Time:%02d:%02d:%02d/End:%4s\n",
        namePlayer, game->rows, game->columns, stats[0], stats[1], stats[2], timePlayed.tm_hour, timePlayed.tm_min, timePlayed.tm_sec, gameEndWrite
    ); // TODO: It raises warning

    //int a = strlen(toWrite);

    fwrite(toWrite, sizeof(char), strlen(toWrite), last10file);

    fclose(last10file);


    last10file = fopen("last10.txt", "rb");
    if (!last10file) {printf("Error opening file."); return;}

    fseek(last10file, 0, SEEK_END);
    int x = ftell(last10file);
    //printf("\nT: %d / D: %d", x, x/(LEN_SAVE10LOG*sizeof(char)));
    int numGamesSaved = x/(LEN_SAVE10LOG*sizeof(char));

    printf("\nA: %d", numGamesSaved);

    if (numGamesSaved > 10) {

        char s[LEN_SAVE10LOG*10];
        fseek(last10file, LEN_SAVE10LOG*sizeof(char), SEEK_SET);
        fread(s, LEN_SAVE10LOG*sizeof(char), 10, last10file);

        //printf("\n%s", &s);

        fclose(last10file);

        last10file = fopen("last10.txt", "wb");
        if (!last10file) {
            printf("\nError creating temp file.");
            return;
        }

        fwrite(s, LEN_SAVE10LOG*sizeof(char), 10, last10file);

        fclose(last10file);
    }
}

void gameStats(GameState* game, int stats[3]) {

    int bombs_pending=0, bombs_right=0, flags=0;

    for (int i=0; i<game->rows; i++) {
        for (int j=0; j<game->columns; j++) {
            mineCeld celd = game->field[i][j];

            if (celd.bomb) celd.flag ? bombs_right++ : bombs_pending++;
            if (celd.flag) flags++;
        }
    }

    stats[0] = bombs_pending;
    stats[1] = bombs_right;
    stats[2] = flags;
}

void loadGameStatsLast10(GameState* game, char dest[10][LEN_SAVE10LOG]) {

    FILE* file = fopen("last10.txt", "rb");
    if (!file) {printf("\nError opening file."); return;}

    for (int i=0; i<10; i++) {
        fread(dest+i, LEN_SAVE10LOG*sizeof(char), 1, file);
        *((*(dest+i))+LEN_SAVE10LOG-1) = '\0';
    }
}



