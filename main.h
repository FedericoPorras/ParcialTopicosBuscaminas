#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "graphics.h"
#include "minesweeper.h"
#include "configs.h"
#include "gamestate.h"

#define WIDTH 1366
#define HEIGHT 768
#define WIDTH_SPACE_MESH_MINES 768

#define FIRST 1
#define MIDDLE 2
#define LAST 3
#define CELD_DIM 23

#define TEX_HIDDEN 9
#define TEX_FLAG 10
#define TEX_BOMB 11
#define TEX_BOMBBOOM 12

#define AMMOUNT_TEXTURES 14
#define AMM_TEXT_COL_ASSET 7
#define AMMOUNT_ASSETS 7

#define CELD_EMPTY 0
#define CELD_BOMB 1

#define OK 0
#define MEM_ERR 1 // TODO: Check when it has to be used!
#define DIM_ERR 2
#define FILE_ERR 3

#define TMP_OFFSET 20

typedef struct Menu { // For later
    void (*init)(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex);
    void (*handler)(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode);
    void (*render)(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex);
} Menu;

#endif // MAIN_H_INCLUDED
