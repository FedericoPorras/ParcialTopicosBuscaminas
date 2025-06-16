#include "menus.h"

// Other things

void renderMeshUpdated(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, GHP_Mesh mesh) {

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
                GHP_Texture adj_tex = tex->textures[TEX_BOMB];
                GHP_renderTexture(renderer, &adj_tex, j*adj_tex.width+mesh.offsetX, i*adj_tex.height+mesh.offsetY);
            } else { // bomb that made the player end the game
                GHP_Texture adj_tex = tex->textures[TEX_BOMBBOOM];
                GHP_renderTexture(renderer, &adj_tex, j*adj_tex.width+mesh.offsetX, i*adj_tex.height+mesh.offsetY);
            }
        }
    }

    if (game->started) {
        int width_cell = tex->textures[TEX_BOMB].width;
        int height_cell = tex->textures[TEX_BOMB].height;
        int i = game->last_selected[0];
        int j = game->last_selected[1];
        mineCeld* selected_cell = &game->field[i][j];
        if (!selected_cell->bomb) {
            SDL_Rect rect = {j*width_cell+mesh.offsetX, i*height_cell+mesh.offsetY, width_cell, height_cell}; // could be any cell of texs, its only for the dimensions
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 50);
            SDL_RenderFillRect(renderer, &rect);
        }

        i = game->hovering[0];
        j = game->hovering[1];
        if (i!=-1 && j!=-1) {
            selected_cell = &game->field[i][j];
            SDL_Rect rect = {j*width_cell+mesh.offsetX, i*height_cell+mesh.offsetY, width_cell, height_cell}; // could be any cell of texs, its only for the dimensions
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 50);
            SDL_RenderFillRect(renderer, &rect);
        }


    }
}

void handleButtonsClick(GHP_Button* buttons, int ammount, int x, int y, GameState* game, int* mode, SDL_Event* event) {
    for (int i=0; i<ammount; i++) {
        if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT && GHP_clickInButton(x, y, buttons+i)) {
            if ((buttons+i)->on_click)
                (buttons+i)->on_click(game, mode);
        }
    }
}

void updateShowLogTexts(SDL_Renderer* renderer, GHP_TexturesData* tex, char type, int i, int j, int year, int mon, int day, int hour, int min, int sec) {
    SDL_Color whiteColor = {255, 255, 255, 255};
    char movementStr[MAX_LEN_LOG];
    sprintf(movementStr, "%s CLICK", type=='R' ? "RIGHT" : "LEFT");
    strcpy(tex->texts[TEXT_SHOWLOG_L1].text, movementStr);
    sprintf(movementStr, "COORDS: (X: %d / Y: %d)", i, j);
    strcpy(tex->texts[TEXT_SHOWLOG_L2].text, movementStr);
    sprintf(movementStr, "%d:%d:%d %d/%d/%d", hour, min, sec, day, mon, year);
    strcpy(tex->texts[TEXT_SHOWLOG_L3].text, movementStr);
    GHP_updateTextTexture(renderer, tex, TEXT_SHOWLOG_L1, 20, whiteColor);
    GHP_updateTextTexture(renderer, tex, TEXT_SHOWLOG_L2, 20, whiteColor);
    GHP_updateTextTexture(renderer, tex, TEXT_SHOWLOG_L3, 20, whiteColor);
}

void updateShowLogTextsEnd(SDL_Renderer* renderer, GHP_TexturesData* tex) {
    SDL_Color whiteColor = {255, 255, 255, 255};
    GHP_updateTextTexture(renderer, tex, TEXT_SHOWLOG_L1, 30, whiteColor);
    GHP_updateTextTexture(renderer, tex, TEXT_SHOWLOG_L2, 30, whiteColor);
    GHP_updateTextTexture(renderer, tex, TEXT_SHOWLOG_L3, 30, whiteColor);
}

void updateGameTime(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* TexData, int mode) {
    if (mode==MODE_PLAY && game->started) {
        struct tm t = timeGame(game->timeStart);

        SDL_Rect rect = {WIDTH_SPACE_MESH_MINES+50, HEIGHT*0.55, TexData->textsTexs[TEXT_GAMETIME].width, TexData->textsTexs[TEXT_GAMETIME].height};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);

        sprintf(TexData->texts[TEXT_GAMETIME].text, "Time: %02d:%02d:%02d", t.tm_hour, t.tm_min, t.tm_sec);
        GHP_updateTextTexture(renderer, TexData, TEXT_GAMETIME, 20, WHITE_COLOR);
        GHP_renderText(renderer, TexData, TEXT_GAMETIME, 30, WHITE_COLOR, WIDTH_SPACE_MESH_MINES+50, HEIGHT*0.55);
        SDL_RenderPresent(renderer);
    }
}

void endGameHandleFiles(GameState* game, int mode) {

    if (mode == MODE_LOST || mode == MODE_WIN) {
        if (game->logFile) {
            char log[12] = "\nEvent:";
            mode == MODE_LOST ? strcat(log, "Lost") : strcat(log, "Win");
            fwrite(log, sizeof(char), strlen(log), game->logFile);
        }

        if (mode == MODE_LOST) fclose(game->logFile);
    }
    else {

        last10Log(game, 3);

        if (game->logFile) {
             fclose(game->logFile);
             game->logFile = NULL;
        }
    }

    saveGameBin(game, game->binFile);
    fclose(game->binFile);
    game->binFile = NULL;
};

void last10Log(GameState* game, int mod) {
    saveGameStatsLast10(game, game->nameUser, mod);
}

void updateLast10GamesTexts(GameState* game, GHP_TexturesData* TexData, char last10GamesData[10][LEN_SAVE10LOG]) {
    for(int i=0; i<10; i++) {
        char name[31], end[] = "    ";
        int rows, cols, mines_pending, mines_discovered, flags, timeHour, timeMin, timeSec;

        sscanf(last10GamesData[i], "Name:%30[^/]/Dimension:%02dx%02d/Mines_pending:%04d/Mines_discovered:%04d/Flags:%04d/Time:%02d:%02d:%02d/End:%4s\n",
            name, &rows, &cols, &mines_pending, &mines_discovered, &flags, &timeHour, &timeMin, &timeSec, end
        );

        sprintf(TexData->texts[TEXT_LAST10GAMES_1+i].text, "%30s | %02dx%02d | %04d | %04d | %04d | %02d:%02d:%02d | %4s",
            name, rows, cols, mines_pending, mines_discovered, flags, timeHour, timeMin, timeSec, end
        );
    }
}

void loadGame(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* TexData, FILE* file, int* mode) {

    int old_dim[] = {game->rows, game->columns};

    fread(&game->started, sizeof(bool), 1, file);
    int headers[8];
    fread(headers, sizeof(int), 8, file);
    game->seed = headers[0];
    game->columns = headers[1];
    game->rows = headers[2];
    game->bombsNum = headers[3];
    game->lost[0] = headers[4];
    game->lost[1] = headers[5];
    game->last_selected[0] = headers[6];
    game->last_selected[1] = headers[7];

    printf("\nGame loaded:");
    printGame(game);

    adjustDim(renderer, game, TexData, old_dim, mode);

    game->field = (mineCeld**)newDinMtx(game->rows, game->columns, sizeof(mineCeld));

    for (int i=0; i<game->rows; i++) {
        for (int j=0; j<game->columns; j++) {
            fread(&game->field[i][j], sizeof(mineCeld), 1, file);
        }
    }

    fread(&game->timeStart, sizeof(size_t), 1, file);

    if (game->started) game->timeFinish = 0;
    else fread(&game->timeFinish, sizeof(size_t), 1, file);

    game->logFile = NULL;
    game->binFile = file;
}

void changeConfig(int rows, int cols, int bombsNum, char perc, int seed) {
    FILE* configFile = fopen("buscaminas.conf", "wt");
    fprintf(configFile, "columnas=%d\nfilas=%d\nminas=%d%c\nsemilla=%d\n", cols, rows, bombsNum, perc=='%' ? '%' : ' ', seed);
    fclose(configFile);
}


// Play

void initPlay(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode) {

    int mode_animation_mesh = 0;

    if (!game->binFile) {

        //nullGame(game);
        initGame(game, configData, tex, renderer);

        //time(&game->timeStart);

        applyConfig(configData, game);
        game->started = false;
        coverAll(game->rows, game->columns, game->field);
        printf("\nGame data loadaded from config.");
        printGame(game);

        char fileName[38];
        char initTimeLog[32];
        time_t t = time(NULL); // current time
        struct tm* now = localtime(&t); // tm format
        strftime(fileName, sizeof(fileName), "games/%Y-%m-%d-%H-%M-%S-gamelog.txt", now);
        sprintf(initTimeLog, "DATE:%d-%d-%d-%d-%d-%d", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec); // no \n because is the first log

        if (!folderExists("games"))
            createFolder("games");

        game->logFile = fopen(fileName, "wt");
        strcpy(fileName+37-7, "dat.dat");
        game->binFile = fopen(fileName, "wb");

        if (!game->binFile) {
            printf("\nBinary file error.");
            *mode = MODE_END;
        }

        if (!game->logFile) {
            printf("\nLog File error.");
            *mode = MODE_END;
        } else
            fwrite(initTimeLog, sizeof(char), strlen(initTimeLog), game->logFile);

        fprintf(game->logFile, "\nDIMENSION:%dx%d\nMINES:%d", game->rows, game->columns, game->bombsNum);

        mode_animation_mesh = 3;

    }
    else {

        loadGame(renderer, game, tex, game->binFile, mode);
        printf("\nGame data already exists. Maybe loaded from file.");
        printGame(game);

        mode_animation_mesh = 2;
    }

    GHP_renderBG(renderer, tex, WIDTH, HEIGHT);
    GHP_renderMesh(renderer, &(tex->active_mesh), mode_animation_mesh);

    GHP_renderButton(renderer, &tex->buttons[BUT_SAVEGAME], WIDTH_SPACE_MESH_MINES + 50, HEIGHT*0.6);
    GHP_renderButton(renderer, &tex->buttons[BUT_MENU], WIDTH_SPACE_MESH_MINES + 50, HEIGHT*0.2);
    GHP_renderText(renderer, tex, TEXT_MINESPENDING, 30, WHITE_COLOR, WIDTH_SPACE_MESH_MINES+50, HEIGHT*0.5);
}

void handlerPlay(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode) {

    GHP_Button buttons[] = {tex->buttons[BUT_MENU]};
    handleButtonsClick(buttons, 1, event->button.x, event->button.y, game, mode, event);

    if (event->type == SDL_MOUSEBUTTONDOWN) {

        if(GHP_clickInMesh(event->button.x, event->button.y, &(tex->active_mesh))) {
            int pos[2];
            GHP_coordsToPos(&(tex->active_mesh), event->button.x, event->button.y, pos);
            memcpy(game->hovering, pos, sizeof(pos));
            int i = *pos; int j = *(pos+1);
            memcpy(game->last_selected, pos, sizeof(pos));

            if (game->started) {
                if (event->button.button == SDL_BUTTON_LEFT) {
                    //if (game->logFile) logFileWriteClick('L', pos, game->logFile);

                    if (userRevealCeld(i, j, game) == CELD_BOMB) {
                        game->started = false;
                        *mode = MODE_LOST;
                    }

                } else if (event->button.button == SDL_BUTTON_RIGHT) {
                    userAuxActionCeld(i, j, game);
                    //if (game->logFile) logFileWriteClick('R', pos, game->logFile);
                }

                if (game->started && checkWin(game->rows, game->columns, game->field)) // TODO: Show it in window
                    *mode = MODE_WIN;

            }
            else {
                if (event->button.button == SDL_BUTTON_LEFT) {
                    // FIRST CELL DISCOVERED
                    time(&game->timeStart);
                    initField(game, pos);
                    userRevealCeld(i, j, game);
                    game->started = true;
                    char logSeed[17]; // maximum int representation has 10 chars length
                    sprintf(logSeed, "\nSEED:%d", game->seed);
                    printf("\nSEED: %d", game->seed);
                    fwrite(logSeed, sizeof(char), strlen(logSeed), game->logFile);
                }


            }

            logFileWriteClick(event->button.button == SDL_BUTTON_LEFT ? 'L' : 'R', pos, game->logFile);

        }

        if (GHP_clickInButton(event->button.x, event->button.y, &tex->buttons[BUT_SAVEGAME])) {
            char msgCloseGame[] = "\nEvent:GameSaved";
            fwrite(msgCloseGame, sizeof(char), strlen(msgCloseGame), game->logFile);
            *mode = MODE_MENU;
        }

        if (GHP_clickInButton(event->button.x, event->button.y, &tex->buttons[BUT_MENU])) {
            char msgCloseGame[] = "\nEvent:PrematureGameEnd";
            fwrite(msgCloseGame, sizeof(char), strlen(msgCloseGame), game->logFile);
            *mode = MODE_MENU;
        }

        if (*mode != MODE_PLAY)
            endGameHandleFiles(game, *mode);

    }

    else if (event->type == SDL_MOUSEMOTION) {
        if (GHP_clickInMesh(event->motion.x, event->motion.y, &(tex->active_mesh))) {
            int pos[2];
            GHP_coordsToPos(&(tex->active_mesh), event->motion.x, event->motion.y, pos);
            memcpy(game->hovering, pos, sizeof(pos));
        }
        else {
            game->hovering[0] = -1;
            game->hovering[1] = -1;
        }
    }
}

void renderPlay(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, int* mode) {
    GHP_renderBG(renderer, tex, WIDTH, HEIGHT); //TODO

    sprintf(tex->texts[TEXT_MINESPENDING].text, "%3d Mines Pending", minesUncovered(game->bombsNum, game->rows, game->columns, game->field) );

    SDL_Rect rect = {WIDTH_SPACE_MESH_MINES+50, HEIGHT*0.5, tex->textsTexs[TEXT_MINESPENDING].width, tex->textsTexs[TEXT_MINESPENDING].height};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect);

    renderMeshUpdated(renderer, game, tex, tex->active_mesh);
    GHP_renderText(renderer, tex, TEXT_MINESPENDING, 30, WHITE_COLOR, WIDTH_SPACE_MESH_MINES+50, HEIGHT*0.5);
    GHP_renderButton(renderer, &tex->buttons[BUT_SAVEGAME], WIDTH_SPACE_MESH_MINES + 50, HEIGHT*0.6);
    GHP_renderButton(renderer, &tex->buttons[BUT_MENU], WIDTH_SPACE_MESH_MINES + 50, HEIGHT*0.2);
}

// Menu

void initMenu(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode) {
    GHP_renderBG(renderer, tex, WIDTH, HEIGHT);
    GHP_renderButton(renderer, &tex->buttons[BUT_START], (WIDTH-289)/2, 30);
    GHP_renderButton(renderer, &tex->buttons[BUT_SEARCHDIR], (WIDTH-(961-597))/2 , HEIGHT*0.7);
    GHP_renderButton(renderer, &tex->buttons[BUT_STATS], (WIDTH-(959-595))/2, HEIGHT*0.5);
    GHP_renderButton(renderer, &tex->buttons[BUT_SETTINGS], (WIDTH-(961-597))/2 , HEIGHT*0.3);
}

void handlerMenu(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode) {
    GHP_Button buttons[] = {tex->buttons[BUT_START], tex->buttons[BUT_SEARCHDIR], tex->buttons[BUT_STATS], tex->buttons[BUT_SETTINGS]};
    handleButtonsClick(buttons, 4, event->button.x, event->button.y, game, mode, event);
}

// Lost

void initLost(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode) {
    GHP_renderBG(renderer, tex, WIDTH, HEIGHT);
    //SDL_RenderPresent(renderer);
    GHP_renderButton(renderer, &tex->buttons[BUT_PLAYAGAIN], 870, HEIGHT*0.15);
    revealAll(game->rows, game->columns, game->field);
    renderMeshUpdated(renderer, game, tex, tex->active_mesh);
}

void handlerLost(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode) {
    GHP_Button buttons[] = {tex->buttons[BUT_PLAYAGAIN]};
    handleButtonsClick(buttons, 1, event->button.x, event->button.y, game, mode, event);
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT && GHP_clickInButton(event->button.x, event->button.y, &tex->buttons[BUT_PLAYAGAIN])) {
        last10Log(game, 2);
        nullGame(game);
    }
}

// Nameplayer

void initNameplayer(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode) {
    GHP_renderBG(renderer, tex, WIDTH, HEIGHT);
    GHP_renderButton(renderer, &tex->buttons[BUT_MENU], (WIDTH-(453-89))/2, 30);

    strcpy(tex->texts[TEXT_NAMEWINNER].text,"\0");

    // white rectangle border
    SDL_Rect rect = {50, 250, WIDTH-50*2, 50};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &rect);
}

void handlerNameplayer(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode) {
    GHP_Button buttons[] = {tex->buttons[BUT_MENU]};
    handleButtonsClick(buttons, 1, event->button.x, event->button.y, game, mode, event);

    if (event->type == SDL_TEXTINPUT) {
        if (strlen(tex->texts[TEXT_NAMEWINNER].text)+1 < MAX_LEN_NAME) {
            strcat(tex->texts[TEXT_NAMEWINNER].text, event->text.text);
            GHP_updateTextTexture(renderer, tex, TEXT_NAMEWINNER, 40, WHITE_COLOR);
        }
    } else if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_BACKSPACE && strlen(tex->texts[TEXT_NAMEWINNER].text) > 0) {
            tex->texts[TEXT_NAMEWINNER].text[strlen(tex->texts[TEXT_NAMEWINNER].text)-1] = '\0';
            GHP_updateTextTexture(renderer, tex, TEXT_NAMEWINNER, 40, WHITE_COLOR);
        }
    }

    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT && GHP_clickInButton(event->button.x, event->button.y, &tex->buttons[BUT_MENU])) {
        fprintf(game->logFile, "\nEvent:Name/Type:N/NameUser:\"%s\"\n", tex->texts[TEXT_NAMEWINNER].text);
        strcpy(game->nameUser, tex->texts[TEXT_NAMEWINNER].text);
        last10Log(game, 1);

        fclose(game->logFile); game->logFile = NULL;
    }
}

void renderNameplayer(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, int* mode) {
    GHP_renderBG(renderer, tex, WIDTH, HEIGHT);
    GHP_renderTexture(renderer, tex->texts[TEXT_NAMEWINNER].tex, 52, 253);
    GHP_renderButton(renderer, &tex->buttons[BUT_MENU], (WIDTH-(453-89))/2, 30);

    // white rectangle border
    SDL_Rect rect = {50, 250, WIDTH-50*2, 50};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &rect);
}

// Win

void initWin(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode) {
    GHP_renderBG(renderer, tex, WIDTH, HEIGHT);
    renderMeshUpdated(renderer, game, tex, tex->active_mesh);
    GHP_renderButton(renderer, &tex->buttons[BUT_SAVENAME], (WIDTH-(961-597))*0.8, HEIGHT-(520-385)-30);

    GHP_renderTextureExpress(renderer, "img/buttons.png", 508, 1284, 1028, 1674, WIDTH_SPACE_MESH_MINES + 50, HEIGHT*0.1);
}

void handlerWin(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode) {
    GHP_Button buttons[] = {tex->buttons[BUT_SAVENAME]};
    handleButtonsClick(buttons, 1, event->button.x, event->button.y, game, mode, event);

    int pos[2][2] = {
        {tex->buttons[BUT_MENU].curWindowX, tex->buttons[BUT_MENU].curWindowY},
        { tex->buttons[BUT_MENU].tex->width + tex->buttons[BUT_MENU].curWindowX,
          tex->buttons[BUT_MENU].tex->height + tex->buttons[BUT_MENU].curWindowY }
    };
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT && GHP_clickIn(event->button.x, event->button.y, pos)) {
        fprintf(game->logFile, "\nEvent:Win/Type:-\n");
        fclose(game->logFile); game->logFile = NULL;
    }
}

// SearchFile

void initSearchFile(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode) {
    strcpy(tex->texts[TEXT_SEARCHFILESENTRY].text, "\0");
    GHP_updateTextTexture(renderer, tex, TEXT_SEARCHFILESENTRY, 40, WHITE_COLOR);
    GHP_renderBG(renderer, tex, WIDTH, HEIGHT);
    GHP_renderButton(renderer, &tex->buttons[BUT_SEARCHDIR], (WIDTH-(961-597))/2 , HEIGHT*0.1);
    GHP_renderButton(renderer, &tex->buttons[BUT_MENU], WIDTH*0.35, HEIGHT*0.7);

    // white rectangle border
    SDL_Rect rect = {50, 250, WIDTH-50*2, 50};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &rect);
}

void handlerSearchFile(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode) {

    GHP_Button buttons[] = {tex->buttons[BUT_MENU]};
    handleButtonsClick(buttons, 1, event->button.x, event->button.y, game, mode, event);

    if (event->type == SDL_TEXTINPUT) {
        if (strlen(tex->texts[TEXT_SEARCHFILESENTRY].text)+1 < GHP_TEXT_LIMIT) {
            strcat(tex->texts[TEXT_SEARCHFILESENTRY].text, event->text.text);
            GHP_updateTextTexture(renderer, tex, TEXT_SEARCHFILESENTRY, 40, WHITE_COLOR);
        }
    } else if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_BACKSPACE && strlen(tex->texts[TEXT_SEARCHFILESENTRY].text) > 0) {
            tex->texts[TEXT_SEARCHFILESENTRY].text[strlen(tex->texts[TEXT_SEARCHFILESENTRY].text)-1] = '\0';
            GHP_updateTextTexture(renderer, tex, TEXT_SEARCHFILESENTRY, 40, WHITE_COLOR);
        }
    }


    if ((event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT && GHP_clickInButton(event->button.x, event->button.y, &tex->buttons[BUT_SEARCHDIR])) ||
        GHP_enterPressed(event)
    ) {

        char fileName[GHP_TEXT_LIMIT+4] = "games/";
        strcat(fileName,tex->texts[TEXT_SEARCHFILESENTRY].text);

        if ( strcmp(".txt", fileName + strlen(fileName) - 4 ) == 0 ) {
            game->logFile = fopen(fileName, "rt");
            if (!game->logFile)
                printf("\nFile %s not found.", fileName);
            else
                *mode = MODE_REPLAY;
        }

        if ( strcmp(".dat", fileName + strlen(fileName) - 4 ) == 0 ) {
            game->binFile = fopen(fileName, "r+b");
            if (!game->binFile)
                printf("\nFile %s not found.", fileName);
            else
                *mode = MODE_PLAY;
        }

    }
}

void renderSearchFile(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, int* mode) {
    GHP_renderBG(renderer, tex, WIDTH, HEIGHT);
    GHP_renderTexture(renderer, tex->texts[TEXT_SEARCHFILESENTRY].tex, 52, 253);
    GHP_renderButton(renderer, &tex->buttons[BUT_SEARCHDIR], (WIDTH-(961-597))/2 , HEIGHT*0.1);
    GHP_renderButton(renderer, &tex->buttons[BUT_MENU], WIDTH*0.35, HEIGHT*0.7);

    GHP_renderTextureExpress(renderer, "img/buttons.png", 72, 1813, 1164, 2024, (WIDTH-(1164-72))/2, HEIGHT*0.4);

    // white rectangle border
    SDL_Rect rect = {50, 250, WIDTH-50*2, 50};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &rect);
}

// Replay

void initReplay(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode) {
    GHP_renderBG(renderer, tex, WIDTH, HEIGHT);
    GHP_renderMesh(renderer, &(tex->active_mesh), false);
    GHP_renderButton(renderer, &tex->buttons[BUT_REPLAYACTION], 1000, (HEIGHT-(763-640))/2);


    int old_dim[] = {game->rows, game->columns};

    bool headers_loaded = false;
    char log[MAX_LEN_LOG];

    int i = 0;
    while(!headers_loaded) {
        fgets(log, MAX_LEN_LOG, game->logFile);
        if (strstr(log, "DIMENSION")) sscanf(log, "DIMENSION:%dx%d\n", &game->rows, &game->columns);
        else if (strstr(log, "MINES")) sscanf(log, "MINES:%d\n", &game->bombsNum);
        else if (strstr(log, "SEED")) {
            sscanf(log, "SEED:%d\n", &game->seed);
            headers_loaded = true;
        }
        i++;
        if (i>10) printf("\nThere are many extra lines in the log file. ");
        if(strstr(log, "PrematureGameEnd")) {
            game->seed = -2;
            headers_loaded = true;
            *mode = MODE_SEARCHDIR;
        }
    }

    adjustDim(renderer, game, tex, old_dim, mode);

    emptyField(game->rows, game->columns, game->field); // necessary for random bombs
    coverAll(game->rows, game->columns, game->field);
    game->started = false;

    printf("\nReplay starts with this game:");
    printGame(game);
}

void handlerReplay(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode) {

    char eventLog[21];
    char type;
    int i, j;
    int year, mon, day, hour, min, sec;
    char format[] = "Event:%[^/]/Type:%c/PosMesh:(%d,%d)/Date:(%d-%d-%d-%d-%d-%d)";

    if ((event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT && GHP_clickInButton(event->button.x, event->button.y, &tex->buttons[BUT_REPLAYACTION]))
        || GHP_enterPressed(event)) {

        char log[MAX_LEN_LOG];
        fgets(log, MAX_LEN_LOG, game->logFile);

        if (sscanf(log, format, &eventLog, &type, &i, &j, &year, &mon, &day, &hour, &min, &sec) >= 1) {

            if (strcmp(eventLog, "Click") == 0) {

                char formatClick[] = "Event:Click/Type:%c/PosMesh:(%d,%d)/Date:(%d-%d-%d-%d-%d-%d)";
                sscanf(log, formatClick, &type, &i, &j, &year, &mon, &day, &hour, &min, &sec);

                int pos[2] = {i, j};
                memcpy(game->last_selected, pos, sizeof(pos));

                if (game->started) {

                    if (type == 'L')
                        userRevealCeld(i, j, game);
                    else if (type == 'R')
                        userAuxActionCeld(i, j, game);
                } else {
                    initField(game, pos);
                    userRevealCeld(i, j, game);
                    //game->started = true;
                }

                updateShowLogTexts(renderer, tex, type, i, j, year, mon, day, hour, min, sec);

            }

            else {
                strcpy(tex->texts[TEXT_SHOWLOG_L1].text, (const char *)&eventLog);
                if (strcmp(eventLog, "Win") == 0) {

                    tex->texts[TEXT_SHOWLOG_L1].text[3] = '\0';
                }
                tex->texts[TEXT_SHOWLOG_L2].text[0] = '\0';
                tex->texts[TEXT_SHOWLOG_L3].text[0] = '\0';

                if (strcmp(eventLog, "Name") == 0) {
                    char formatName[] = "Event:Name/Type:N/NameUser:\"%[^\"]\"";
                    sscanf(log, formatName, (char*)&tex->texts[TEXT_SHOWLOG_L2].text);
                }
            }

        } else { // no more logs -> game ended
            fclose(game->logFile);
            *mode = MODE_SEARCHDIR;
            game->started = false;
        }

    }

}

void renderReplay(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, int* mode) {
    GHP_renderBG(renderer, tex, WIDTH, HEIGHT);
    renderMeshUpdated(renderer, game, tex, tex->active_mesh);
    updateShowLogTextsEnd(renderer, tex);

    if (game->started) {
        GHP_renderTexture(renderer, tex->texts[TEXT_SHOWLOG_L1].tex, WIDTH_SPACE_MESH_MINES + 50, 100);
        GHP_renderTexture(renderer, tex->texts[TEXT_SHOWLOG_L2].tex, WIDTH_SPACE_MESH_MINES + 50, 125);
        GHP_renderTexture(renderer, tex->texts[TEXT_SHOWLOG_L3].tex, WIDTH_SPACE_MESH_MINES + 50, 150);
    }

    GHP_renderButton(renderer, &tex->buttons[BUT_REPLAYACTION], 1000, (HEIGHT-(763-640))/2);
}

// Stats

void initStats(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode) {
    GHP_renderBG(renderer, tex, WIDTH, HEIGHT);
    GHP_renderButton(renderer, &tex->buttons[BUT_MENU], (WIDTH-tex->buttons[BUT_MENU].tex->width)/2, HEIGHT*0.7);

    char last10GamesData[10][LEN_SAVE10LOG];
    int numGamesLoaded = loadGameStatsLast10(game, last10GamesData);
    updateLast10GamesTexts(game, tex, last10GamesData);
    strcpy((char *)&tex->texts[TEXT_LAST10GAMES_HEAD].text, "             NAME              |  DIM  | M.PEN| M.DIS| FLAGS|   TIME   | END ");

    int i;

    int fontNum = 30;

    for (i=0; i<numGamesLoaded; i++) // = cause of the head
        GHP_updateTextTexture(renderer, tex, TEXT_LAST10GAMES_1+i, fontNum, WHITE_COLOR);
    GHP_updateTextTexture(renderer, tex, TEXT_LAST10GAMES_HEAD, fontNum, WHITE_COLOR);

    for (i=0; i<numGamesLoaded; i++)
        GHP_renderTexture(renderer, tex->texts[TEXT_LAST10GAMES_1+i].tex, 25, HEIGHT*0.5-30*i);
    GHP_renderTexture(renderer, tex->texts[TEXT_LAST10GAMES_HEAD].tex, 25, HEIGHT*0.5-30*i);

}

void handlerStats(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode) {
    GHP_Button buttons[] = {tex->buttons[BUT_MENU]};
    handleButtonsClick(buttons, 1, event->button.x, event->button.y, game, mode, event);
}

// Settings

void initSettings(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode) {
    strcpy(tex->texts[TEXT_SETTINGS].text, "\0");

    GHP_updateTextTexture(renderer, tex, TEXT_SETTINGS, 40, WHITE_COLOR);
    GHP_renderBG(renderer, tex, WIDTH, HEIGHT);
    GHP_renderButton(renderer, &tex->buttons[BUT_MENU], WIDTH*0.3, HEIGHT*0.7);

    // white rectangle border
    SDL_Rect rect = {50, 250, WIDTH-50*2, 50};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &rect);
}

void handlerSettings(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode) {
    GHP_Button buttons[] = {tex->buttons[BUT_MENU]};
    handleButtonsClick(buttons, 1, event->button.x, event->button.y, game, mode, event);

    if (event->type == SDL_TEXTINPUT) {
        if (strlen(tex->texts[TEXT_SETTINGS].text)+1 < MAX_LEN_SETTINGS_TEXT &&
            INPUTCHARSETVAL(event->text.text[strlen(event->text.text)-1])
            ) {
            strcat(tex->texts[TEXT_SETTINGS].text, event->text.text);
            GHP_updateTextTexture(renderer, tex, TEXT_SETTINGS, 40, WHITE_COLOR);
        }

    } else if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_BACKSPACE && strlen(tex->texts[TEXT_SETTINGS].text) > 0) {
            tex->texts[TEXT_SETTINGS].text[strlen(tex->texts[TEXT_SETTINGS].text)-1] = '\0';
            GHP_updateTextTexture(renderer, tex, TEXT_SETTINGS, 40, WHITE_COLOR);
        }

    }

    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT
        && GHP_clickInButton(event->button.x, event->button.y, &tex->buttons[BUT_MENU])) {

        int rows=0, cols=0, mines=0;
        char perc='\0';
        if (sscanf(tex->texts[TEXT_SETTINGS].text, "%dx%d/%d%c", &rows, &cols, &mines, &perc) >= 3)
            changeConfig(rows, cols, mines, perc, -1);

    }
}

void renderSettings(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, int* mode) {
    GHP_updateTextTexture(renderer, tex, TEXT_SETTINGS, 40, WHITE_COLOR);
    GHP_renderBG(renderer, tex, WIDTH, HEIGHT);
    GHP_renderButton(renderer, &tex->buttons[BUT_MENU], WIDTH*0.35, HEIGHT*0.7);

    GHP_renderTextureExpress(renderer, "img/buttons.png", 72, 2058, 1164, 2268, (WIDTH-(1164-72))/2, HEIGHT*0.4);

    // white rectangle border
    SDL_Rect rect = {50, 250, WIDTH-50*2, 50};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &rect);

    GHP_renderText(renderer, tex, TEXT_SETTINGS, 40, WHITE_COLOR, 50, 250);
}



