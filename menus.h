#ifndef MENUS_H_INCLUDED
#define MENUS_H_INCLUDED

#include "constants.h"
#include "dirHandle.h"
#include "graphics.h"
#include "gamestate.h"
#include "configs.h"
#include "minesweeper.h"
#include "init.h"

#ifndef _WIN32
#include <windows.h>
#endif // _WIN32


typedef struct Section {
    void (*init)(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode);
    void (*handler)(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode);
    void (*render)(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, int* mode);
} Section;

// other
void renderMeshUpdated(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, GHP_Mesh mesh);
void handleButtonsClick(GHP_Button* buttons, int ammount, int x, int y, GameState* game, int* mode, SDL_Event* event);
void updateShowLogTexts(SDL_Renderer* renderer, GHP_TexturesData* tex, char type, int i, int j, int year, int mon, int dat, int hour, int min, int sec);
void updateGameTime(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* TexData, int mode);
void endGameHandleFiles(GameState* game, int mode);
void last10Log(GameState* game, int mod);
void loadGame(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* TexData, FILE* file, int* mode);

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
void initStats(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode);
void handlerStats(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode);

#endif // MENUS_H_INCLUDED
