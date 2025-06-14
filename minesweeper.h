#ifndef MINESWEEPER_H_INCLUDED
#define MINESWEEPER_H_INCLUDED

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "gamestate.h"


#define CELD_EMPTY 0
#define CELD_BOMB 1
#define CELD_FLAGGED 2


void emptyField(int rows, int cols, mineCeld** field);
int randomBombs(int rows, int cols, mineCeld** field, int ammount, int seed, int pos[2]);
void printFieldConsole(int rows, int cols, mineCeld** field);
char check_position(int i, int j, int rows, int cols);
void calcAdjacency(int rows, int cols, mineCeld** field);
void revealAll(int rows, int cols, mineCeld** field);
void revealAdjacencies(int i, int j , int rows, int cols, mineCeld** field);
bool checkNoAdjacencies(int i, int j, int rows, int cols, mineCeld** field);
bool checkWin(int rows, int cols, mineCeld** field);
void coverAll(int rows, int cols, mineCeld** field);
void initField(GameState* game, int pos[2]);
int userRevealCeld(int i, int j, GameState* game);
void userAuxActionCeld(int i, int j, GameState* game);
void logFileWriteClick(char button, int* posInMesh, FILE* file);
int minesUncovered(int bombsNum, int rows, int cols, mineCeld** field);

#endif // MINESWEEPER_H_INCLUDED
