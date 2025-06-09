#include "main.h"
#include <windows.h> // TODO DELETE

// TODO: void autoFlag / When you click and the only option is to be a flag in a celd to fill it with the flag
// this could be an extra functionality
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
        {initNameplayer, handlerNameplayer, renderNameplayer}
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
                    char msgCloseGame[] = "\nGame premature end";
                    fwrite(msgCloseGame, sizeof(char), strlen(msgCloseGame), game.logFile);
                    fclose(game.logFile);
                    free(game.field);
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

    destroyDinMtx(game.rows, game.columns, sizeof(mineCeld), (void**)game.field);
}






bool initGame(GameState* game, ConfigData* configs) {

    game->field = (mineCeld**)newDinMtx(game->rows, game->columns, sizeof(mineCeld));
    if (!game->field) {
        printf("\nMemory Error. There is no memory for the field.");
        return false;
    }

    emptyField(game->rows, game->columns, game->field);

    return true;
}

void initField(GameState* game, int pos[2]) {
    emptyField(game->rows, game->columns, game->field);
    game->seed = randomBombs(game->rows, game->columns, game->field, game->bombsNum, game->seed, pos);
    calcAdjacency(game->rows, game->columns, game->field);
    game->started = true;
}

int initTexData(GHP_TexturesData* tex_data, SDL_Renderer* renderer, GameState* game) { // TODO: put it on another place

    // choose appropriate dimension
    int stdDimGrid[] = { 8, 10, 15, 18, 24, 32, 40};
    int stdDimPix[]  = {76, 59, 48, 38, 29, 22, 16};
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
    GHP_newButtonAbs(renderer, "img/buttons.png", texData, &texData->buttons[BUT_START], 0, 0, 289, 153, (WIDTH-289)/2, 30, setModePlay);
    GHP_newButtonAbs(renderer, "img/buttons.png", texData, &texData->buttons[BUT_PLAYAGAIN], 328, 34, 653, 308, 1000, (HEIGHT-(308-34))/2, setModeMenu);
    GHP_newButtonAbs(renderer, "img/buttons.png", texData, &texData->buttons[BUT_NAMEPLAYER], 722, 39, 1086, 250, (WIDTH-289)/2, 30+153+30, setModeNameplayer);

    for(int i=0; i<AMMOUNT_BUTTONS; i++) {
        if (! (texData->buttons + i)->tex ) {
            printf("\nError loading the buttons.");
            return TEX_ERR; // could be file
        }
    }
    return OK;
}

int initTexts(SDL_Renderer* renderer, GHP_TexturesData* texData) {
    SDL_Color myColor = {255, 255, 255, 255};
    GHP_newText(renderer, "ttf/Consolas-Regular.ttf", texData, &(texData->texts[0]), 600, 300, 20, myColor);

    for(int i=0; i<AMMOUNT_TEXTS; i++) {
        if (! (texData->texts + i)->tex ) {
            printf("\nError loading the buttons.");
            return TEX_ERR; // could be file
        }
    }
    return OK;
}

void** newDinMtx(int rows, int cols, int len) {
    void** rv = malloc(8 * rows); // 8 because pointers always have 8 bytes length (game for 64 bit)
    if (!rv) {
        printf("\nMalloc Error. There is no memory for the field.");
        return NULL;
    }
    for (int i=0; i<rows; i++) {
        *(rv+i) = malloc(len * cols);
        if (!*(rv+i)) {
            printf("\nMalloc Error. There is no memory for the field.");
            for (int j=0; j<i; j++)
                free(*(rv+j));
            free(rv);
            return NULL;
        }
    }
    return rv;
}

void destroyDinMtx(int rows, int cols, int len, void** mtx) {
    for (int i=0; i<rows; i++) {
        free(*(mtx+i));
    }
    free(mtx);
}

int userRevealCeld(int i, int j, GameState* game) {
    if (!game->field[i][j].bomb) {
        game->field[i][j].revealed = true;
        revealAdjacencies(i, j, game->rows, game->columns, game->field);
        return CELD_EMPTY;
    } else if (!game->field[i][j].flag) {
        revealAll(game->rows, game->columns, game->field);
        game->lost[0] = i; game->lost[1] = j;
        return CELD_BOMB;
    } else return CELD_FLAGGED;
}

void userAuxActionCeld(int i, int j, GameState* game) {
    if (game->field[i][j].flag) game->field[i][j].flag = false;
    else game->field[i][j].flag = true;
}

void logFileWriteClick(char button, int* posInMesh, FILE* file) {
    char date[20];
    time_t t = time(NULL);
    struct tm* now = localtime(&t);
    sprintf(date, "%04d-%02d-%02d-%02d-%02d-%02d", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec); // TODO: Release warning

    fprintf(file, "\nEvent:Click/Type:%c/PosMesh:(%02d,%02d)/Date:(%s)", button, *posInMesh, *(posInMesh+1), date);
}

void renderMeshUpdated(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, GHP_Mesh mesh) {
    // REMEMBER: To print you put backwards the i and j. REVIEW WHY BECAUSE I DONT REMEMBER. TODO

    GHP_Texture hidden = tex->textures[9];
    GHP_Texture flag = tex->textures[TEX_FLAG];

    for (int i=0; i<game->rows; i++) {
        for (int j=0; j<game->columns; j++) {
            if (!game->field[i][j].revealed) {
                if (game->field[i][j].flag) GHP_renderTexture(renderer, &flag, j*flag.width+mesh.offsetX, i*flag.height+mesh.offsetY);
                else GHP_renderTexture(renderer, &hidden, j*hidden.width+mesh.offsetX, i*hidden.height+mesh.offsetY);
            }
            else if (!game->field[i][j].bomb)
            {
                GHP_Texture adj_tex = tex->textures[game->field[i][j].adjacency];
                GHP_renderTexture(renderer, &adj_tex, j*adj_tex.width+mesh.offsetX, i*adj_tex.height+mesh.offsetY);
            } else if (i != game->lost[0] || j != game->lost[1]) {
                GHP_Texture adj_tex = tex->textures[TEX_BOMB]; // TODO: Change name
                GHP_renderTexture(renderer, &adj_tex, j*adj_tex.width+mesh.offsetX, i*adj_tex.height+mesh.offsetY);
            } else { // bomb that made the player end the game
                GHP_Texture adj_tex = tex->textures[TEX_BOMBBOOM]; // TODO: Change name
                GHP_renderTexture(renderer, &adj_tex, j*adj_tex.width+mesh.offsetX, i*adj_tex.height+mesh.offsetY);
            }
        }
    }
}

void handleButtonsClick(GHP_Button* buttons, int ammount, int x, int y, GameState* game, int* mode, SDL_Event* event) {
    for (int i=0; i<ammount; i++) {
        int pos[2][2] = {
            { (buttons+i)->windowX, (buttons+i)->windowY },
            { (buttons+i)->windowX + (buttons+i)->tex->width,
              (buttons+i)->windowY + (buttons+i)->tex->height }
        };
        if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT && GHP_clickIn(x, y, pos)) {
            (buttons+i)->on_click(game, mode);
        }
    }
}




// Play

void initPlay(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode) {

    // start the game
    applyConfig(configData, game);
    game->started = false;
    coverAll(game->rows, game->columns, game->field);

    char logFileName[32];
    char initTimeLog[32];
    time_t t = time(NULL); // current time
    struct tm* now = localtime(&t); // tm format
    strftime(logFileName, sizeof(logFileName), "%Y-%m-%d-%H-%M-%S-gamelog.txt", now);
    sprintf(initTimeLog, "DATE:%d-%d-%d-%d-%d-%d", now->tm_year+1900, now->tm_mon, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec); // no \n because is the first log
    game->logFile = fopen(logFileName, "wb");

    if (!game->logFile) {
        printf("\nFile error.");
        *mode = MODE_END;
    } else
        fwrite(initTimeLog, sizeof(char), strlen(initTimeLog), game->logFile);


    // log() -> time(START, END, GAME_TIME) pos(X,Y) action(PRESSED/FLAGGED/UNFLAGGED)
    // log_seed() -> SEED XXXXX

    GHP_setBGColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    GHP_renderMesh(renderer, &(tex->active_mesh), false);
}

void handlerPlay(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode) {

    if (event->type == SDL_MOUSEBUTTONDOWN) {

        if(GHP_clickInMesh(event->button.x, event->button.y, &(tex->active_mesh))) {
            int pos[2];
            GHP_coordsToPos(&(tex->active_mesh), event->button.x, event->button.y, pos);
            int i = *pos; int j = *(pos+1);

            if (game->started) {
                if (event->button.button == SDL_BUTTON_LEFT) {
                    logFileWriteClick('L', pos, game->logFile);
                    if (userRevealCeld(i, j, game) == CELD_BOMB) {
                        game->started = false;
                        char logLose[] = "\nEND_GAME:game_lost";
                        fwrite(logLose, sizeof(char), strlen(logLose), game->logFile);
                        fclose(game->logFile);
                        *mode = MODE_LOST;
                    }

                } else if (event->button.button == SDL_BUTTON_RIGHT) {
                    userAuxActionCeld(i, j, game);
                    logFileWriteClick('R', pos, game->logFile);
                }

                if (game->started && checkWin(game->rows, game->columns, game->field)) // TODO: Show it in window
                    printf("\n\n\nWIN!");
            } else {
                // to avoid first click bomb
                initField(game, pos);
                userRevealCeld(i, j, game);
                game->started = true;
                char logSeed[17]; // maximum int representation has 10 chars length
                sprintf(logSeed, "\nSEED:%d", game->seed);
                fwrite(logSeed, sizeof(char), strlen(logSeed), game->logFile);
            }
        }

    }
}

void renderPlay(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, int* mode) {

    // TODO: Only re-render the changed celds
    renderMeshUpdated(renderer, game, tex, tex->active_mesh);
}

// Menu

void initMenu(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode) {
    GHP_setBGColor(renderer, 255, 0, 0, 255);
    GHP_renderButton(renderer, &tex->buttons[BUT_START]);
    GHP_renderButton(renderer, &tex->buttons[BUT_NAMEPLAYER]);


}

void handlerMenu(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode) {
    GHP_Button buttons[] = {tex->buttons[BUT_START], tex->buttons[BUT_NAMEPLAYER]};
    handleButtonsClick(buttons, AMMOUNT_BUTTONS, event->button.x, event->button.y, game, mode, event);
    /*
    int pos[2][2] = {{50, 50}, {50+289, 50+153}};
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT
        && GHP_clickIn(event->button.x, event->button.y, pos))
        *mode = MODE_PLAY;
    */
}

// Lost

void initLost(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode) {
    GHP_Texture texButPlayAgain = GHP_newTextureAbs(renderer, "img/buttons.png", 328, 34, 653, 308);
    GHP_renderTexture(renderer, &texButPlayAgain, 1000, 300);
    revealAll(game->rows, game->columns, game->field);
    renderMeshUpdated(renderer, game, tex, tex->active_mesh);
    SDL_RenderPresent(renderer);
}

void handlerLost(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode) {
    GHP_Button buttons[] = {tex->buttons[BUT_PLAYAGAIN]};
    handleButtonsClick(buttons, AMMOUNT_BUTTONS, event->button.x, event->button.y, game, mode, event);
    /*
    int pos[2][2] = { {1000, 300}, {1000 + 653 - 328, 300 + 308 - 34} };
    if (event->button.button == SDL_BUTTON_LEFT) {
        if (GHP_clickIn(event->button.x, event->button.y, pos))
            *mode = MODE_MENU;
    }
    */
}

// Nameplayer

void initNameplayer(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode) {
    SDL_StartTextInput();
    GHP_setBGColor(renderer, 0, 0, 255, 255);
    GHP_renderButton(renderer, &tex->buttons[BUT_START]);
}

void handlerNameplayer(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode) {
    GHP_Button buttons[] = {tex->buttons[BUT_START]};
    handleButtonsClick(buttons, AMMOUNT_BUTTONS, event->button.x, event->button.y, game, mode, event);

    if (event->type == SDL_TEXTINPUT) {
        if (strlen(tex->texts[0].text)+1 < GHP_TEXT_LIMIT) {
            strcat(tex->texts[0].text, event->text.text);
            printf("\n (%02d) TEXT: %s", (int)strlen(tex->texts[0].text) ,tex->texts[0].text);
            SDL_Color myColor = {255, 255, 255, 255};
            GHP_updateTextTexture(renderer, tex, 0, 40, myColor);
        }
    } else if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_BACKSPACE && strlen(tex->texts[0].text) > 0) {
            printf("\nChar deleted");
            tex->texts[0].text[strlen(tex->texts[0].text)-1] = '\0';
            printf("\n (%02d) TEXT: %s", (int)strlen(tex->texts[0].text) ,tex->texts[0].text);
            SDL_Color myColor = {255, 255, 255, 255};
            GHP_updateTextTexture(renderer, tex, 0, 40, myColor);
        }
    }

}

void renderNameplayer(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, int* mode) {
    GHP_setBGColor(renderer, 0, 0, 255, 255);
    GHP_renderButton(renderer, &tex->buttons[BUT_START]);
    GHP_renderTexture(renderer, tex->texts->tex, 750, 300);
}


    /*
    int pos[2][2] = { {1000, 300}, {1000 + 653 - 328, 300 + 308 - 34} };
    if (event->button.button == SDL_BUTTON_LEFT) {
        if (GHP_clickIn(event->button.x, event->button.y, pos))
            *mode = MODE_MENU;
    }
    */




// Buttons

// typedef void (*ButtonReaction)(void* gameData, int* mode);
void setModePlay(void* gameData, int* mode) {SDL_StartTextInput(); *mode = MODE_PLAY;}
void setModeMenu(void* gameData, int* mode) {*mode = MODE_MENU;}
void setModeLost(void* gameData, int* mode) {*mode = MODE_LOST;}
void setModeNameplayer(void* gameData, int* mode) {*mode = MODE_NAMEPLAYER;}


