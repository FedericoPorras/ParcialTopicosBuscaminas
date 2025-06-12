#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "dinMtx.h"
#include "graphics.h"
#include "gamestate.h"
#include "configs.h"
#include "minesweeper.h"
#include "menus.h"


#define WIDTH 1366
#define HEIGHT 768
#define WIDTH_SPACE_MESH_MINES 768

#define FIRST 1
#define MIDDLE 2
#define LAST 3

#define OK 0
#define MEM_ERR 1 // TODO: Check when it has to be used!
#define DIM_ERR 2
#define FILE_ERR 3
#define GRAL_ERR 4
#define TEX_ERR 5


// generals
bool initGame(GameState* game, ConfigData* configs);
void draw_init(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex_data);
int initTexData(GHP_TexturesData* tex_data, SDL_Renderer* renderer, GameState* game);
void** newDinMtx(int rows, int cols, int len);
void destroyDinMtx(int rows, int cols, int len, void** mtx);
void react(SDL_Renderer* renderer, void* gameData, GHP_TexturesData* TexData);
void draw_menu(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* TexData);
int initButtons(SDL_Renderer* renderer, GHP_TexturesData* buttons);
int initTexts(SDL_Renderer* renderer, GHP_TexturesData* texData);

#endif // MAIN_H_INCLUDED
