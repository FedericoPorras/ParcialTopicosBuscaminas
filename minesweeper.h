#ifndef MINESWEEPER_H_INCLUDED
#define MINESWEEPER_H_INCLUDED

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "gamestate.h"

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

#endif // MINESWEEPER_H_INCLUDED
