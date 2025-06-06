#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "graphics/graphics.h"

#define INIT_DIM 10
#define FIRST 1
#define MIDDLE 2
#define LAST 3
#define CELD_DIM 23

#define TEX_HIDDEN 9
#define TEX_FLAG 10
#define TEX_BOMB 11
#define TEX_BOMBBOOM 12

#define AMMOUNT_TEXTURES 13

#define MAX_LEN_OPTION 51

#define MIN_BOMBS 1
#define MAX_BOMBS 1000
#define MIN_PER 0.01
#define MAX_PER 100
#define MIN_COLS 1
#define MAX_COLS 50
#define MIN_ROWS 1
#define MAX_ROWS 50 // TODO: Check this limits!


typedef struct {
    bool bomb;
    bool flag;
    bool question_mark; // to be implemented
    bool revealed;
    bool loose; // TODO: CHANGE IT! DELETE IT!
    int adjacency;
} mineCeld; // maybe then add a cheat or sth

typedef struct {
    int columns;
    int rows;
    int bombsNum;
    float bombsPer; // percentage
    mineCeld squareField10[10][10];
    mineCeld* field;
} GameState;

#endif // MAIN_H_INCLUDED
