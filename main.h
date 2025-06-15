#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "constants.h"
#include "init.h"
#include "dinMtx.h"
#include "graphics.h"
#include "gamestate.h"
#include "configs.h"
#include "minesweeper.h"
#include "menus.h"


// generals
//void draw_init(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex_data);
void** newDinMtx(int rows, int cols, int len);
void destroyDinMtx(int rows, int cols, int len, void** mtx);
void react(SDL_Renderer* renderer, void* gameData, GHP_TexturesData* TexData);
//void draw_menu(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* TexData);

#endif // MAIN_H_INCLUDED
