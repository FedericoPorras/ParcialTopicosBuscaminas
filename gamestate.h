#ifndef GAMESTATE_H_INCLUDED
#define GAMESTATE_H_INCLUDED

#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include "dinMtx.h"

typedef struct {
    bool bomb;
    bool flag;
    bool question_mark; // to be implemented
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
} GameState;

void nullGame(GameState* game);
void printGame(GameState* game);
void saveGame(GameState* game, FILE* file);
void loadGame(GameState* game, FILE* file);


#endif // GAMESTATE_H_INCLUDED

