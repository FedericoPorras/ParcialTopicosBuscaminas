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

typedef struct {
    bool bomb;
    bool flag;
    bool question_mark; // to be implemented
    bool revealed;
    bool loose; // TODO: CHANGE IT! DELETE IT!
    int adjacency;
} mineCeld; // maybe then add a cheat or sth

typedef struct {
    mineCeld squareField8[8][8];
    mineCeld squareField10[10][10];
    mineCeld squareField15[15][15];
    mineCeld squareField20[20][20];
    mineCeld squareField32[32][32];
} GameState;

#endif // MAIN_H_INCLUDED
