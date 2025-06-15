#ifndef GAMESTATE_H_INCLUDED
#define GAMESTATE_H_INCLUDED

#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "dinMtx.h"

#define MAX_LEN_NAME 30
#define LEN_SAVE10LOG 127

typedef struct {
    bool bomb;
    bool flag;
    bool revealed;
    int adjacency;
} mineCeld; // maybe then add a cheat or sth

typedef struct {
    bool started;
    int seed;
    int columns;
    int rows;
    int bombsNum;
    int lost[2];
    mineCeld** field;
    FILE* logFile;
    FILE* binFile;
    time_t timeStart;
    time_t timeFinish;
    int last_selected[2];
    char nameUser[MAX_LEN_NAME];
    int hovering[2];
} GameState;

void nullGame(GameState* game);
void printGame(GameState* game);
void saveGameBin(GameState* game, FILE* file);
struct tm timeGame(time_t timeG);
void saveGameStats(GameState* game, FILE* last10file, char* namePlayer, int gameEnd);
void gameStats(GameState* game, int stats[3]);
void loadGameStatsLast10(GameState* game, char dest[10][LEN_SAVE10LOG]);
void saveGameStatsLast10(GameState* game, char* namePlayer, int gameEnd);

#endif // GAMESTATE_H_INCLUDED

