#include "init.h"

bool initGame(GameState* game, ConfigData* configs, GHP_TexturesData* TexData, SDL_Renderer* renderer) {

    if (!initConfig(configs)) {
        if (!resetConfig()) {
            return false;
        }
        initConfig(configs);
    }

    applyConfig(configs, game);

    game->binFile = NULL;

    if (!initMtx(game)) return false;

    emptyField(game->rows, game->columns, game->field);

    strcpy(game->nameUser, "<Anonymous User>");

    if (initTexData(TexData, renderer, game) != OK)
        return false;

    return true;
}

int initTexData(GHP_TexturesData* tex_data, SDL_Renderer* renderer, GameState* game) {

    if (!GHP_setBG(renderer, tex_data, PATH_BG)) {
        return FILE_ERR;
    }

    // choose appropriate dimension
    int stdDimGrid[] = { 8, 10, 15, 18, 24, 32, 40};
    int stdDimPix[]  = {76, 58, 48, 38, 29, 22, 16}; // TODO: CHECK
    int dimGridTex = 0, i = 0;
    while (!dimGridTex && i<AMMOUNT_ASSETS) {
        if (game->columns > stdDimGrid[i] || game->rows > stdDimGrid[i]) i++;
        else dimGridTex = stdDimGrid[i];
    }
    if (i >= AMMOUNT_ASSETS) {
        printf("\nDimension error. There is no asset matchable with the dimension chosen.");
        return DIM_ERR;
    }

    // load textures
    char path[19];
    sprintf(path, "img/celds%dx%d.png", stdDimGrid[i], stdDimGrid[i]);
    while (GHP_loadRectAsset(renderer, path, &(tex_data->textures), AMMOUNT_TEXTURES, stdDimPix[i], stdDimPix[i], AMM_TEXT_COL_ASSET) != OK && i<= AMMOUNT_ASSETS ){
        // at least try to render other asset
        printf("\nFile of field %s not found, trying to use another asset.", path);
        i++;
        sprintf(path, "img/celds%dx%d.png", stdDimGrid[i], stdDimGrid[i]);
        //GHP_loadRectAsset(renderer, path, &(tex_data->textures), AMMOUNT_TEXTURES, stdDimPix[i], stdDimPix[i], AMM_TEXT_COL_ASSET);
    }

    if (i > AMMOUNT_ASSETS) {
        printf("\nDimension error");
        return FILE_ERR;
    }

    if (!tex_data->textures) {
        printf("\nError loading textures.");
        return MEM_ERR;
    }

    tex_data->textures_loaded = AMMOUNT_TEXTURES;


    // set mesh
    int offset_to_centerX = (WIDTH_SPACE_MESH_MINES - game->columns * stdDimPix[i]) / 2;
    int offset_to_centerY = (HEIGHT - game->rows * stdDimPix[i]) / 2;
    tex_data->active_mesh = (GHP_Mesh){offset_to_centerX, offset_to_centerY, &(tex_data->textures[TEX_HIDDEN]), game->rows, game->columns};

    // mallocs for buttons and texts
    tex_data->buttons = malloc(sizeof(GHP_Button)*AMMOUNT_BUTTONS);
    tex_data->buttonsTexs = malloc(sizeof(GHP_Texture)*AMMOUNT_BUTTONS);
    tex_data->buttons_loaded = 0;

    tex_data->texts = malloc(sizeof(GHP_Text)*AMMOUNT_TEXTS);
    tex_data->textsTexs = malloc(sizeof(GHP_Texture)*AMMOUNT_TEXTS);
    tex_data->texts_loaded = 0;

    if (initButtons(renderer, tex_data) != OK) return TEX_ERR;
    if (initTexts(renderer, tex_data) != OK) return TEX_ERR;

    return OK;
}

int initButtons(SDL_Renderer* renderer, GHP_TexturesData* texData) {
    GHP_newButtonAbs(renderer, "img/buttons.png", texData, &texData->buttons[BUT_START], 0, 0, 289, 153, setModePlay); // (WIDTH-289)/2, 30
    GHP_newButtonAbs(renderer, "img/buttons.png", texData, &texData->buttons[BUT_PLAYAGAIN], 328, 34, 653, 308, setModeMenu); // 1000, (HEIGHT-(308-34))/2,
    GHP_newButtonAbs(renderer, "img/buttons.png", texData, &texData->buttons[BUT_NAMEPLAYER], 722, 39, 1086, 250, setModeMenu); // 250, (WIDTH-289)/2, 30+153+30
    GHP_newButtonAbs(renderer, "img/buttons.png", texData, &texData->buttons[BUT_MENU], 89, 382, 453, 593, setModeMenu); // (WIDTH-(453-89))/2, (HEIGHT-(593-328))*0.75,
    GHP_newButtonAbs(renderer, "img/buttons.png", texData, &texData->buttons[BUT_SAVENAME], 597, 385, 961, 520, setModeNameplayer); // (WIDTH-(961-597))/2, HEIGHT-(520-385)-30,
    GHP_newButtonAbs(renderer, "img/buttons.png", texData, &texData->buttons[BUT_SEARCHDIR], 597, 565, 961, 700, setModeSearchDir);
    GHP_newButtonAbs(renderer, "img/buttons.png", texData, &texData->buttons[BUT_REPLAYACTION], 97, 640, 228, 763, NULL);
    GHP_newButtonAbs(renderer, "img/buttons.png", texData, &texData->buttons[BUT_SAVEGAME], 596, 734, 960, 869, setModePlay);
    GHP_newButtonAbs(renderer, "img/buttons.png", texData, &texData->buttons[BUT_STATS], 595, 890, 959, 1025, setModeStats);
    GHP_newButtonAbs(renderer, "img/buttons.png", texData, &texData->buttons[BUT_SETTINGS], 596, 1068, 958, 1203, setModeSettings);

    for(int i=0; i<AMMOUNT_BUTTONS; i++) {
        if (! (texData->buttons + i)->tex ) {
            printf("\nError loading the buttons. Button %d.", i);
            return TEX_ERR; // could be file
        }
    }
    return OK;
}

int initTexts(SDL_Renderer* renderer, GHP_TexturesData* texData) {
    SDL_Color whiteColor = {255, 255, 255, 255};
    char path[] = "fnt/Consolas-Regular.ttf";
    for (int i=0; i<AMMOUNT_TEXTS; i++) {
        GHP_newText(renderer, path, texData, &(texData->texts[i]), -1, -1, -1, whiteColor);
        texData->texts[i].text[0] = '\0';
    }

    for(int i=0; i<AMMOUNT_TEXTS; i++) {
        if (! (texData->texts + i)->tex ) {
            printf("\nError loading the textures. Texture %d.", i);
            return TEX_ERR;
        }
    }
    return OK;
}

bool initMtx(GameState* game) {
    game->field = (mineCeld**)newDinMtx(game->rows, game->columns, sizeof(mineCeld));
    if (!game->field) {
        printf("\nMemory Error. There is no memory for the field.");
        return false;
    }
    return true;
}

void adjustDim(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* TexData, int dim[2], int* mode) {
    if (game->rows != dim[0] && game->columns != dim[1])
        if (!initMtx(game) || initTexData(TexData, renderer, game) != OK)
            *mode = MODE_END;
}

// Buttons Reactions
void setModePlay(void* gameData, int* mode) {*mode = MODE_PLAY;}
void setModeMenu(void* gameData, int* mode) {*mode = MODE_MENU;}
void setModeLost(void* gameData, int* mode) {*mode = MODE_LOST;}
void setModeNameplayer(void* gameData, int* mode) { *mode = MODE_NAMEPLAYER; }
void setModeSearchDir(void* gameData, int*mode) {*mode = MODE_SEARCHDIR; }
void setModeStats(void* gameData, int*mode) {*mode = MODE_STATS; }
void setModeSettings(void* gameData, int* mode) {*mode = MODE_SETTINGS;}
