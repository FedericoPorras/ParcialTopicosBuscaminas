#ifndef GAMESTATE_H_INCLUDED
#define GAMESTATE_H_INCLUDED

#include <stdbool.h>
#include <stdio.h>

typedef struct {
    bool bomb;
    bool flag;
    bool question_mark; // to be implemented
    bool revealed;
    bool loose; // TODO: CHANGE IT! DELETE IT!
    int adjacency;
} mineCeld; // maybe then add a cheat or sth

typedef struct {
    bool started;
    int seed;
    int columns;
    int rows;
    int bombsNum;
    float bombsPer; // percentage
    mineCeld** field;
} GameState;


void nullGame(GameState* game); // TODO: Use it, replace other aparitions
void printGame(GameState* game);

#endif // GAMESTATE_H_INCLUDED
