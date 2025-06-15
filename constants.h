#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

#define MAX_LEN_LOG 100

#define MODE_STATS 7
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
#define BUT_STATS 8
#define AMMOUNT_BUTTONS 9

#define TEXT_SEARCHFILESENTRY 0
#define TEXT_SHOWLOG_L1 1
#define TEXT_SHOWLOG_L2 2
#define TEXT_SHOWLOG_L3 3
#define TEXT_MINESPENDING 4
#define TEXT_GAMETIME 5
#define TEXT_LAST10GAMES_1 6
#define TEXT_LAST10GAMES_2 7
#define TEXT_LAST10GAMES_3 8
#define TEXT_LAST10GAMES_4 9
#define TEXT_LAST10GAMES_5 10
#define TEXT_LAST10GAMES_6 11
#define TEXT_LAST10GAMES_7 12
#define TEXT_LAST10GAMES_8 13
#define TEXT_LAST10GAMES_9 14
#define TEXT_LAST10GAMES_10 15
#define TEXT_LAST10GAMES_HEAD 16
#define AMMOUNT_TEXTS 17


#define WIDTH 1366
#define HEIGHT 768
#define WIDTH_SPACE_MESH_MINES 768

#define WHITE_COLOR (SDL_Color){255, 255, 255, 255}



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


#endif // CONSTANTS_H_INCLUDED
