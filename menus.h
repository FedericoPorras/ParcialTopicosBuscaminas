#ifndef MENUS_H_INCLUDED
#define MENUS_H_INCLUDED

#include "dirHandle.h"
#include "graphics.h"
#include "gamestate.h"
#include "configs.h"
#include "minesweeper.h"

#define MAX_LEN_LOG 100

#define MODE_REPLAY 6
#define MODE_SEARCHDIR 5
#define MODE_WIN 4
#define MODE_NAMEPLAYER 3
#define MODE_LOST 2
#define MODE_PLAY 1
#define MODE_MENU 0
#define MODE_END -1

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
#define BUT_SEARCHDIR 5
#define BUT_REPLAYACTION 6
#define BUT_SAVEGAME 7
#define AMMOUNT_BUTTONS 8

#define TEXT_SEARCHFILESENTRY 0
#define TEXT_SHOWLOG_L1 1
#define TEXT_SHOWLOG_L2 2
#define TEXT_SHOWLOG_L3 3
#define AMMOUNT_TEXTS 4


#define WIDTH 1366
#define HEIGHT 768
#define WIDTH_SPACE_MESH_MINES 768


typedef struct Section {
    void (*init)(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode);
    void (*handler)(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode);
    void (*render)(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, int* mode);
} Section;

// other
void renderMeshUpdated(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, GHP_Mesh mesh);
void handleButtonsClick(GHP_Button* buttons, int ammount, int x, int y, GameState* game, int* mode, SDL_Event* event);
void updateShowLogTexts(SDL_Renderer* renderer, GHP_TexturesData* tex, char type, int i, int j, int year, int mon, int dat, int hour, int min, int sec);

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
void initSearchFile(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode);
void handlerSearchFile(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode);
void renderSearchFile(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, int* mode);
void initReplay(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode);
void handlerReplay(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode);
void renderReplay(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, int* mode);
void handleButtonsClick(GHP_Button* buttons, int ammount, int x, int y, GameState* game, int* mode, SDL_Event* event);


// for buttons reactions
void setModePlay(void* gameData, int* mode);
void setModeMenu(void* gameData, int* mode);
void setModeLost(void* gameData, int* mode);
void setModeNameplayer(void* gameData, int* mode);
void setModeSearchDir(void* gameData, int*mode);



#endif // MENUS_H_INCLUDED
