#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "graphics.h" //
#include "gamestate.h"
#include "configs.h"
#include "minesweeper.h"


#define WIDTH 1366
#define HEIGHT 768
#define WIDTH_SPACE_MESH_MINES 768

#define MODE_WIN 4
#define MODE_NAMEPLAYER 3
#define MODE_LOST 2
#define MODE_PLAY 1
#define MODE_MENU 0
#define MODE_END -1

#define FIRST 1
#define MIDDLE 2
#define LAST 3

#define TEX_HIDDEN 9
#define TEX_FLAG 10
#define TEX_BOMB 11
#define TEX_BOMBBOOM 12
#define AMMOUNT_TEXTURES 14
#define AMM_TEXT_COL_ASSET 7
#define AMMOUNT_ASSETS 7

#define BUT_START 0
#define BUT_PLAYAGAIN 1
#define BUT_NAMEPLAYER 2
#define BUT_MENU 3
#define BUT_SAVENAME 4
#define AMMOUNT_BUTTONS 5

#define AMMOUNT_TEXTS 1

#define CELD_EMPTY 0
#define CELD_BOMB 1
#define CELD_FLAGGED 2

#define OK 0
#define MEM_ERR 1 // TODO: Check when it has to be used!
#define DIM_ERR 2
#define FILE_ERR 3
#define GRAL_ERR 4
#define TEX_ERR 5

typedef struct Section {
    void (*init)(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode);
    void (*handler)(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode);
    void (*render)(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, int* mode);
} Section;


// generals
bool initGame(GameState* game, ConfigData* configs);
void draw_init(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex_data);
int initTexData(GHP_TexturesData* tex_data, SDL_Renderer* renderer, GameState* game);
void** newDinMtx(int rows, int cols, int len);
void destroyDinMtx(int rows, int cols, int len, void** mtx);
void initField(GameState* game, int pos[2]);
void react(SDL_Renderer* renderer, void* gameData, GHP_TexturesData* TexData);
int userRevealCeld(int i, int j, GameState* game);
void userAuxActionCeld(int i, int j, GameState* game);
void draw_menu(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* TexData);
void logFileWriteClick(char button, int* posInMesh, FILE* file);
void renderMeshUpdated(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, GHP_Mesh mesh);
void handleButtonsClick(GHP_Button* buttons, int ammount, int x, int y, GameState* game, int* mode, SDL_Event* event);
int initButtons(SDL_Renderer* renderer, GHP_TexturesData* buttons);
int initTexts(SDL_Renderer* renderer, GHP_TexturesData* texData);

// for menus
void initPlay(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode);
void handlerPlay(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode);
void renderPlay(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, int* mode);
void initMenu(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode);
void handlerMenu(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode);
void initLost(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode);
void handlerLost(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode);
void initNameplayer(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode);
void handlerNameplayer(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode);
void renderNameplayer(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, int* mode);
void initWin(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode);
void handlerWin(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode);
void initReplay(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode);
void handlerWin(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode);

// for buttons
void setModePlay(void* gameData, int* mode);
void setModeMenu(void* gameData, int* mode);
void setModeLost(void* gameData, int* mode);
void setModeNameplayer(void* gameData, int* mode);

#endif // MAIN_H_INCLUDED
