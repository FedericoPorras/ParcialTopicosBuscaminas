#include "main.h"

bool initGame(GameState* game);
void draw_init(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex_data);
void draw_update_play(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex_data); // TODO: Error msgs
int initTexData(GHP_TexturesData* tex_data, SDL_Renderer* renderer, GameState* game);
void** newDinMtx(int rows, int cols, int len);
int initField(GameState* game, int pos[2]);
void draw(SDL_Renderer* renderer, void* gameData, GHP_TexturesData* TexData, int* mode);
void react(SDL_Renderer* renderer, void* gameData, GHP_TexturesData* TexData, Drawing draw);
int userRevealCeld(int i, int j, GameState* game);
void userAuxActionCeld(int i, int j, GameState* game);
void draw_menu(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* TexData);
void draw_init_play(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* TexData);

// TODO: If you put the prototypes in the .h file the compiler does not find GHP



// TODO: void autoFlag / When you click and the only option is to be a flag in a celd to fill it with the flag
// this could be an extra functionality
// also a REQUIRED functionality is that if the user clicks a revealed num, it must free adjacencies due to
// having the enough flags for its own adjacency, and if the flags were wrong, lose the game

int main (int argc, char *argv[]) {

    GameState game;
    if (!initGame(&game)) {
        printf("\nError in game initalizing");
        return 1;
    }

    GHP_TexturesData tex_data;

    char* nameWindow = "Buscaminas";
    struct GHP_WindowData myWindow;
    if (GHP_SetWindow(&myWindow, nameWindow, react, draw, WIDTH, HEIGHT, &game, &tex_data)) { // it returns true at the end of the game
        GHP_DestroyWindow(&myWindow);
        free(tex_data.textures);
    }

    //srand(time(NULL));

    getchar(); // avoid closing the cmd

    return 0;
}

void react(SDL_Renderer* renderer, void* gameData, GHP_TexturesData* TexData, Drawing draw) {

    GHP_setBGColor(renderer, 255, 0, 0, 255);

    GameState* game = (GameState*) gameData; // TODO: what with that?

    SDL_Event event;
    int mode = MODE_INIT_MENU;

    if (initTexData(TexData, renderer, game) != OK)
        mode = MODE_END;

    char logFileName[50];
    time_t t = time(NULL);
    struct tm* now = localtime(&t);
    strftime(logFileName, sizeof(logFileName), "%y-%m-%d-%H-%M-%S-gamelog.txt", now);
    printf("\n%s", logFileName);

    //  TODO
    //  FILE* logFile = fopen()
    // log() -> time(START, END, GAME_TIME) pos(X,Y) action(PRESSED/FLAGGED/UNFLAGGED)
    // log_seed() -> SEED XXXXX

    while (mode != MODE_END) { // TODO: Put in only one condition if necessary at the end of the project

        // TODO: Organize by events or modes ?

        printf("\nHELP: MODE: %d", mode);

        while (SDL_PollEvent(&event)) {

            //printf("\nHELP: EVENT %s", event.type);

            if (event.type == SDL_QUIT) { // window 'X' button
                mode=MODE_END;
                printf("\nQUIT BUTTON PRESSED\n");
            }

            else if (mode == MODE_PLAY) {

                if (event.type == SDL_MOUSEBUTTONDOWN) {

                    if(GHP_clickInMesh(event.button.x, event.button.y, &(TexData->active_mesh))) {
                            int pos[2];
                            GHP_coordsToPos(&(TexData->active_mesh), event.button.x, event.button.y, pos);
                            int i = *pos; int j = *(pos+1);

                        if (game->started) {
                            if (event.button.button == SDL_BUTTON_LEFT) {
                                if (userRevealCeld(i, j, game) == CELD_BOMB)
                                    mode = MODE_INIT_LOST;
                            }

                            else if (event.button.button == SDL_BUTTON_RIGHT)
                                userAuxActionCeld(i, j, game);

                            if (checkWin(game->rows, game->columns, game->field)) // TODO: Show it in window
                                printf("\n\n\nWIN!");
                        } else {
                            // to avoid first click bomb
                            int seed = initField(game, pos);
                            userRevealCeld(i, j, game);
                        }
                    }

                }

            }

            else if (mode == MODE_MENU) {

                if (event.type == SDL_MOUSEBUTTONDOWN) {

                    int pos[2];
                    GHP_coordsToPos(&(TexData->active_mesh), event.button.x, event.button.y, pos);
                    int i = *pos; int j = *(pos+1);

                    if (event.button.button == SDL_BUTTON_LEFT) {

                        //if (GHP_clickIn())
                            mode = MODE_INIT_PLAY;

                    }

                }

            }

            else if (mode == MODE_LOST) {

                printf("B");

                if (event.type == SDL_MOUSEBUTTONDOWN) {

                    int pos[2];
                    GHP_coordsToPos(&(TexData->active_mesh), event.button.x, event.button.y, pos);
                    int i = *pos; int j = *(pos+1);

                    if (event.button.button == SDL_BUTTON_LEFT) {

                        //if (GHP_clickIn())
                            mode = MODE_INIT_MENU;

                    }

                }
            }

        }

        draw(renderer, game, TexData, &mode);

        if (mode == MODE_INIT_PLAY) {
            game->started = false;
            coverAll(game->rows, game->columns, game->field);
            draw_init_play(renderer, game, TexData);
            mode = MODE_PLAY;
        }

        if (mode == MODE_INIT_LOST) {
            GHP_Texture texButPlayAgain = GHP_newTextureAbs(renderer, "img/buttons.png", 328, 34, 653, 308);
            GHP_renderTexture(renderer, &texButPlayAgain, 1000, 300);
            SDL_RenderPresent(renderer);
            mode = MODE_LOST;
        }

    }

}

void draw(SDL_Renderer* renderer, void* gameData, GHP_TexturesData* TexData, int* mode) {
    GameState* game = (GameState*)gameData;

    switch(*mode) {
        case MODE_PLAY:
        case MODE_INIT_LOST:
            draw_update_play(renderer, game, TexData);
            break;
        case MODE_INIT_MENU:
            draw_menu(renderer, game, TexData);
            *mode = MODE_MENU;
            break;
    }
    SDL_RenderPresent(renderer);
}

bool initGame(GameState* game) {
    if (!initConfig(game)) {
        resetConfig();
        initConfig(game);
    }
    printGame(game);

    game->field = (mineCeld**)newDinMtx(game->rows, game->columns, sizeof(mineCeld));
    if (!game->field) {
        printf("\nMemory Error. There is no memory for the field.");
        return false;
    }

    emptyField(game->rows, game->columns, game->field);

    return true;
}

int initField(GameState* game, int pos[2]) {
    emptyField(game->rows, game->columns, game->field);
    int seed = randomBombs(game->rows, game->columns, game->field, game->bombsNum, game->seed, pos);
    calcAdjacency(game->rows, game->columns, game->field);
    game->started = true;
    return seed;
}

void draw_update_play(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex_data) {
    // REMEMBER: To print you put backwards the i and j. REVIEW WHY BECAUSE I DONT REMEMBER. TODO

    GHP_Mesh mesh = tex_data->active_mesh;
    GHP_Texture hidden = tex_data->textures[9];
    GHP_Texture flag = tex_data->textures[TEX_FLAG];

    for (int i=0; i<game->rows; i++) {
        for (int j=0; j<game->columns; j++) {
            if (!game->field[i][j].revealed) {
                if (game->field[i][j].flag) GHP_renderTexture(renderer, &flag, j*flag.width+mesh.offsetX, i*flag.height+mesh.offsetY);
                else GHP_renderTexture(renderer, &hidden, j*hidden.width+mesh.offsetX, i*hidden.height+mesh.offsetY);
            }
            else if (!game->field[i][j].bomb)
            {
                GHP_Texture adj_tex = tex_data->textures[game->field[i][j].adjacency];
                GHP_renderTexture(renderer, &adj_tex, j*adj_tex.width+mesh.offsetX, i*adj_tex.height+mesh.offsetY);
            } else if (!game->field[i][j].loose) { // bomb that made the player end the game
                GHP_Texture adj_tex = tex_data->textures[TEX_BOMB]; // TODO: Change name
                GHP_renderTexture(renderer, &adj_tex, j*adj_tex.width+mesh.offsetX, i*adj_tex.height+mesh.offsetY);
            } else { // bomb loose
                GHP_Texture adj_tex = tex_data->textures[TEX_BOMBBOOM]; // TODO: Change name
                GHP_renderTexture(renderer, &adj_tex, j*adj_tex.width+mesh.offsetX, i*adj_tex.height+mesh.offsetY);
            }
        }
    }

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
        printf("\nDimension error");
        return DIM_ERR;
    }

    // load textures
    char path[19];
    sprintf(path, "img/celds%dx%d.png", stdDimGrid[i], stdDimGrid[i]);
    while (GHP_loadRectAsset(renderer, path, &(tex_data->textures), AMMOUNT_TEXTURES, stdDimPix[i], stdDimPix[i], AMM_TEXT_COL_ASSET) != OK && i<= AMMOUNT_ASSETS ) {
        // at least try to render other asset
        printf("\nFile of field %s not found, trying to use another one asset.", path);
        i++;
        sprintf(path, "img/celds%dx%d.png", stdDimGrid[i], stdDimGrid[i]);
        GHP_loadRectAsset(renderer, path, &(tex_data->textures), AMMOUNT_TEXTURES, stdDimPix[i], stdDimPix[i], AMM_TEXT_COL_ASSET);
    }

    if (i > AMMOUNT_ASSETS)
        return FILE_ERR;

    if (!tex_data->textures) {
        return MEM_ERR;
    }

    // set mesh
    int offset_to_centerX = (WIDTH_SPACE_MESH_MINES - game->columns * stdDimPix[i]) / 2;
    int offset_to_centerY = (HEIGHT - game->rows * stdDimPix[i]) / 2;
    tex_data->active_mesh = (GHP_Mesh){offset_to_centerX, offset_to_centerY, &(tex_data->textures[TEX_HIDDEN]), game->rows, game->columns};

    return OK;
}

void** newDinMtx(int rows, int cols, int len) {
    void** rv = malloc(len * rows); // TODO: This could be replaced by 8. pointers have the same size, type does not matter
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

int userRevealCeld(int i, int j, GameState* game) {
    if (!game->field[i][j].bomb) {
        game->field[i][j].revealed = true;
        revealAdjacencies(i, j, game->rows, game->columns, game->field);
        return CELD_EMPTY;
    } else {
        game->field[i][j].loose = true;
        revealAll(game->rows, game->columns, game->field);
        return CELD_BOMB;
    }
}

void userAuxActionCeld(int i, int j, GameState* game) {
    if (game->field[i][j].flag) game->field[i][j].flag = false;
    else game->field[i][j].flag = true;
}

void draw_menu(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* TexData) {

    GHP_setBGColor(renderer, 0, 0, 0, 255);

    GHP_Texture texButStart = GHP_newTexture(renderer, "img/buttons.png", 0, 0, 289, 153);
    GHP_renderTexture(renderer, &texButStart, 50, 50);

}

void draw_init_play(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* TexData) {
    GHP_setBGColor(renderer, 0, 0, 0, 255);
    GHP_renderMesh(renderer, &(TexData->active_mesh), false);
}


