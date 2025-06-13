#include "main.h"
#include <windows.h> // TODO DELETE

// also a REQUIRED functionality is that if the user clicks a revealed num, it must free adjacencies due to
// having the enough flags for its own adjacency, and if the flags were wrong, lose the game



int main (int argc, char *argv[]) {

    ConfigData configs;

    GHP_TexturesData tex_data;

    char* nameWindow = "Buscaminas";
    struct GHP_WindowData myWindow;
    if (GHP_SetWindow(&myWindow, nameWindow, react, WIDTH, HEIGHT, &configs, &tex_data)) { // it returns true at the end of the game
        GHP_DestroyWindow(&myWindow);
        GHP_DestroyTexturesData(&tex_data);
    }

    //srand(time(NULL));

    getchar(); // avoid closing the cmd

    return 0;
}

void react(SDL_Renderer* renderer, void* gameData, GHP_TexturesData* TexData) {

    int mode = MODE_MENU, modePrev; // init mode

    GHP_setBGColor(renderer, 0, 255, 0, 255); // for debugging

    ConfigData* configs = (ConfigData*) gameData;
    if (!initConfig(configs)) {
        if (!resetConfig()) {
            mode=MODE_END;
        }
        initConfig(configs);
    }

    GameState game;
    nullGame(&game);
    applyConfig(configs, &game);
    if (!initGame(&game, configs)) {
        printf("\nError in game initalizing");
        mode=MODE_END;
    }

    if (initTexData(TexData, renderer, &game) != OK)
        mode = MODE_END;

    Section SectionPool[] = {
        {initMenu, handlerMenu, NULL},
        {initPlay, handlerPlay, renderPlay},
        {initLost, handlerLost, NULL},
        {initNameplayer, handlerNameplayer, renderNameplayer},
        {initWin, handlerWin, NULL},
        {initSearchFile, handlerSearchFile, renderSearchFile},
        {initReplay, handlerReplay, renderReplay}
    };


    if (mode != MODE_END) {
        SectionPool[mode].init(renderer, &game, TexData, configs, &mode);
        SDL_RenderPresent(renderer);
    }

    SDL_Event event;
    while (mode != MODE_END) {

        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                mode = MODE_END;
                if (game.started) {
                    char msgCloseGame[] = "\nEvent:PrematureGameEnd";
                    fwrite(msgCloseGame, sizeof(char), strlen(msgCloseGame), game.logFile);
                }
            }

            else {

                modePrev = mode;
                SectionPool[mode].handler(renderer, &game, TexData, &event, &mode);

                if (modePrev != mode) { // init function only when the mode changes

                    SDL_Event discard;
                    while (SDL_PollEvent(&discard)) {} // clear the queue of events

                    SectionPool[mode].init(renderer, &game, TexData, configs, &mode);
                }

                if (SectionPool[mode].render) { // because some modes have no render
                    SectionPool[mode].render(renderer, &game, TexData, &mode);
                }

                SDL_RenderPresent(renderer);
            }

        }

    }

    fclose(game.logFile); // ASK For save any unexpected error
    fclose(game.binFile);
    destroyDinMtx(game.rows, game.columns, sizeof(mineCeld), (void**)game.field);
}


bool initGame(GameState* game, ConfigData* configs) {

    game->binFile = NULL;

    game->field = (mineCeld**)newDinMtx(game->rows, game->columns, sizeof(mineCeld));
    if (!game->field) {
        printf("\nMemory Error. There is no memory for the field.");
        return false;
    }

    emptyField(game->rows, game->columns, game->field);

    return true;
}

int initTexData(GHP_TexturesData* tex_data, SDL_Renderer* renderer, GameState* game) { // TODO: put it on another place

    // choose appropriate dimension
    int stdDimGrid[] = { 8, 10, 15, 18, 24, 32, 40};
    int stdDimPix[]  = {76, 58, 48, 38, 29, 22, 16};
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
        printf("\nDimension error"); // TODO: Check this error
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

    for(int i=0; i<AMMOUNT_BUTTONS; i++) {
        if (! (texData->buttons + i)->tex ) {
            printf("\nError loading the buttons. Button %d.", i);
            return TEX_ERR; // could be file
        }
    }
    return OK;
}

int initTexts(SDL_Renderer* renderer, GHP_TexturesData* texData) {
    SDL_Color myColor = {255, 255, 255, 255};
    GHP_newText(renderer, "ttf/Consolas-Regular.ttf", texData, &(texData->texts[TEXT_SEARCHFILESENTRY]), 52, 253, 20, myColor);
    GHP_newText(renderer, "ttf/Consolas-Regular.ttf", texData, &(texData->texts[TEXT_SHOWLOG_L1]), 1000, 100, 20, myColor);
    GHP_newText(renderer, "ttf/Consolas-Regular.ttf", texData, &(texData->texts[TEXT_SHOWLOG_L2]), 1000, 200, 20, myColor);
    GHP_newText(renderer, "ttf/Consolas-Regular.ttf", texData, &(texData->texts[TEXT_SHOWLOG_L3]), 1000, 300, 20, myColor);

    for(int i=0; i<AMMOUNT_TEXTS; i++) {
        if (! (texData->texts + i)->tex ) {
            printf("\nError loading the textures. Texture %d.", i);
            return TEX_ERR; // TODO could be file
        }
    }
    return OK;
}


