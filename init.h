#ifndef INIT_H_INCLUDED
#define INIT_H_INCLUDED

#include <SDL.h>
#include <stdbool.h>
#include <stdlib.h>

#include "constants.h"
#include "gamestate.h"
#include "configs.h"
#include "graphics.h"
#include "minesweeper.h"

bool initGame(GameState* game, ConfigData* configs, GHP_TexturesData* TexData, SDL_Renderer* renderer);
int initButtons(SDL_Renderer* renderer, GHP_TexturesData* buttons);
int initTexts(SDL_Renderer* renderer, GHP_TexturesData* texData);
int initTexData(GHP_TexturesData* tex_data, SDL_Renderer* renderer, GameState* game);
bool initMtx(GameState* game);
void adjustDim(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* TexData, int dim[2], int* mode);

// for buttons reactions
void setModePlay(void* gameData, int* mode);
void setModeMenu(void* gameData, int* mode);
void setModeLost(void* gameData, int* mode);
void setModeNameplayer(void* gameData, int* mode);
void setModeSearchDir(void* gameData, int*mode);
void setModeStats(void* gameData, int*mode);



#endif // INIT_H_INCLUDED
