#include "menus.h"


// Other things

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
        if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT && GHP_clickInButton(x, y, buttons+i)) {
            if ((buttons+i)->on_click) // TODO: Join the condition
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


// Play

void initPlay(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode) {

    if (!game->binFile) {

        applyConfig(configData, game);
        game->started = false;
        coverAll(game->rows, game->columns, game->field);

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

        fprintf(game->logFile, "\nDIMENSION:%dx%d\nMINES:%d", game->rows, game->columns, game->bombsNum); // TODO: Join both texts

    } else {

        loadGame(game, game->binFile);

    }

    // TODO
    // log() -> time(START, END, GAME_TIME) pos(X,Y) action(PRESSED/FLAGGED/UNFLAGGED)
    // log_seed() -> SEED XXXXX

    GHP_setBGColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    GHP_renderMesh(renderer, &(tex->active_mesh), false);
    GHP_renderButton(renderer, &tex->buttons[BUT_SAVEGAME], WIDTH_SPACE_MESH_MINES + 50, HEIGHT*0.6);
    GHP_renderButton(renderer, &tex->buttons[BUT_MENU], WIDTH_SPACE_MESH_MINES + 50, HEIGHT*0.2);
}

void handlerPlay(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode) {

    GHP_Button buttons[] = {tex->buttons[BUT_MENU]};
    handleButtonsClick(buttons, 1, event->button.x, event->button.y, game, mode, event);

    if (event->type == SDL_MOUSEBUTTONDOWN) {

        if(GHP_clickInMesh(event->button.x, event->button.y, &(tex->active_mesh))) {
            int pos[2];
            GHP_coordsToPos(&(tex->active_mesh), event->button.x, event->button.y, pos);
            int i = *pos; int j = *(pos+1);

            if (game->started) {
                if (event->button.button == SDL_BUTTON_LEFT) {
                    if (game->logFile) logFileWriteClick('L', pos, game->logFile);

                    if (userRevealCeld(i, j, game) == CELD_BOMB) {
                        game->started = false;
                        if (game->logFile) {
                            char logLose[] = "\nEvent:Lost";
                            fwrite(logLose, sizeof(char), strlen(logLose), game->logFile);
                            fclose(game->logFile); game->logFile = NULL;
                        }
                        saveGame(game, game->binFile);
                        *mode = MODE_LOST;
                    }

                } else if (event->button.button == SDL_BUTTON_RIGHT) {
                    userAuxActionCeld(i, j, game);
                    if (game->logFile) logFileWriteClick('R', pos, game->logFile);
                }

                if (game->started && checkWin(game->rows, game->columns, game->field)) // TODO: Show it in window
                    *mode = MODE_WIN;
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

        if (GHP_clickInButton(event->button.x, event->button.y, &tex->buttons[BUT_SAVEGAME])) {
            saveGame(game, game->binFile);
            if (game->logFile) fclose(game->logFile);
            nullGame(game);
            *mode = MODE_MENU;
        }

        if (GHP_clickInButton(event->button.x, event->button.y, &tex->buttons[BUT_MENU])) { // TODO: Menu should not save the game? Also think that when SDL_QUIT it is saving the file
            char msgCloseGame[] = "\nEvent:PrematureGameEnd";
            fwrite(msgCloseGame, sizeof(char), strlen(msgCloseGame), game->logFile);
            saveGame(game, game->binFile);
            *mode = MODE_MENU;

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
    GHP_renderButton(renderer, &tex->buttons[BUT_START], (WIDTH-289)/2, 30);
    GHP_renderButton(renderer, &tex->buttons[BUT_SEARCHDIR], (WIDTH-(961-597))/2 , HEIGHT*0.7);
}

void handlerMenu(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode) {
    GHP_Button buttons[] = {tex->buttons[BUT_START], tex->buttons[BUT_SEARCHDIR]};
    handleButtonsClick(buttons, 2, event->button.x, event->button.y, game, mode, event);
    /*
    int pos[2][2] = {{50, 50}, {50+289, 50+153}};
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT
        && GHP_clickIn(event->button.x, event->button.y, pos))
        *mode = MODE_PLAY;
    */
}

// Lost

void initLost(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode) {
    GHP_renderButton(renderer, &tex->buttons[BUT_PLAYAGAIN], 870, HEIGHT*0.15);
    revealAll(game->rows, game->columns, game->field);
    renderMeshUpdated(renderer, game, tex, tex->active_mesh);
    SDL_RenderPresent(renderer);
}

void handlerLost(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode) {
    GHP_Button buttons[] = {tex->buttons[BUT_PLAYAGAIN]};
    handleButtonsClick(buttons, 1, event->button.x, event->button.y, game, mode, event);
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT && GHP_clickInButton(event->button.x, event->button.y, &tex->buttons[BUT_PLAYAGAIN]))
        nullGame(game);

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
    GHP_renderButton(renderer, &tex->buttons[BUT_MENU], (WIDTH-(453-89))/2, 30);

    // white rectangle border
    SDL_Rect rect = {50, 250, WIDTH-50*2, 50};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &rect);
}

void handlerNameplayer(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode) {
    GHP_Button buttons[] = {tex->buttons[BUT_MENU]};
    handleButtonsClick(buttons, 1, event->button.x, event->button.y, game, mode, event);

    if (event->type == SDL_TEXTINPUT) {
        if (strlen(tex->texts[TEXT_SEARCHFILESENTRY].text)+1 < GHP_TEXT_LIMIT) {
            strcat(tex->texts[TEXT_SEARCHFILESENTRY].text, event->text.text);
            SDL_Color myColor = {255, 255, 255, 255};
            GHP_updateTextTexture(renderer, tex, 0, 40, myColor);
        }
    } else if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_BACKSPACE && strlen(tex->texts[TEXT_SEARCHFILESENTRY].text) > 0) {
            tex->texts[TEXT_SEARCHFILESENTRY].text[strlen(tex->texts[TEXT_SEARCHFILESENTRY].text)-1] = '\0';
            SDL_Color myColor = {255, 255, 255, 255};
            GHP_updateTextTexture(renderer, tex, 0, 40, myColor);
        }
    }

    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT && GHP_clickInButton(event->button.x, event->button.y, &tex->buttons[BUT_MENU])) {
        fprintf(game->logFile, "\nEvent:Win/Type:N/NameUser:\"%s\"\n", tex->texts[TEXT_SEARCHFILESENTRY].text);
        fclose(game->logFile); game->logFile = NULL;
        SDL_StopTextInput();
    }
}

void renderNameplayer(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, int* mode) {
    GHP_setBGColor(renderer, 0, 0, 255, 255);
    GHP_renderTexture(renderer, tex->texts->tex, 52, 253);
    GHP_renderButton(renderer, &tex->buttons[BUT_MENU], (WIDTH-(453-89))/2, 30);

    // white rectangle border
    SDL_Rect rect = {50, 250, WIDTH-50*2, 50};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &rect);
}

// Win

void initWin(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode) {
    renderMeshUpdated(renderer, game, tex, tex->active_mesh);
    GHP_renderButton(renderer, &tex->buttons[BUT_MENU], (WIDTH-(453-89))*0.8, 30);
    GHP_renderButton(renderer, &tex->buttons[BUT_SAVENAME], (WIDTH-(961-597))*0.8, HEIGHT-(520-385)-30);
}

void handlerWin(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode) {
    GHP_Button buttons[] = {tex->buttons[BUT_MENU], tex->buttons[BUT_SAVENAME]};
    handleButtonsClick(buttons, 2, event->button.x, event->button.y, game, mode, event);

    int pos[2][2] = {
        {tex->buttons[BUT_MENU].curWindowX, tex->buttons[BUT_MENU].curWindowY},
        { tex->buttons[BUT_MENU].tex->width + tex->buttons[BUT_MENU].curWindowX,
          tex->buttons[BUT_MENU].tex->height + tex->buttons[BUT_MENU].curWindowY }
    };
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT && GHP_clickIn(event->button.x, event->button.y, pos)) {
        fprintf(game->logFile, "\nEvent:Win/Type:-\n"); // ASK fprintf when unnecessary vs fwrite
        fclose(game->logFile); game->logFile = NULL;
        nullGame(game);
    }
}

// SearchFile

void initSearchFile(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode) {
    SDL_StartTextInput();
    strcpy(tex->texts[TEXT_SEARCHFILESENTRY].text, "\0");
    SDL_Color myColor = {255, 255, 255, 255}; // TODO: Remove it
    GHP_updateTextTexture(renderer, tex, 0, 40, myColor);
    GHP_setBGColor(renderer, 0, 0, 255, 255);
    GHP_renderButton(renderer, &tex->buttons[BUT_SEARCHDIR], (WIDTH-(961-597))/2 , HEIGHT*0.1);
    GHP_renderButton(renderer, &tex->buttons[BUT_MENU], WIDTH*0.3, HEIGHT*0.7);

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
            SDL_Color myColor = {255, 255, 255, 255}; // TODO: Remove it
            GHP_updateTextTexture(renderer, tex, TEXT_SEARCHFILESENTRY, 40, myColor);
        }
    } else if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_BACKSPACE && strlen(tex->texts[TEXT_SEARCHFILESENTRY].text) > 0) {
            tex->texts[TEXT_SEARCHFILESENTRY].text[strlen(tex->texts[TEXT_SEARCHFILESENTRY].text)-1] = '\0';
            SDL_Color myColor = {255, 255, 255, 255}; // TODO: Remove it
            GHP_updateTextTexture(renderer, tex, TEXT_SEARCHFILESENTRY, 40, myColor);
        }
    }

    int pos[2][2] = { // TODO: Replace this kind of things with GHP_clickInButton
        {tex->buttons[BUT_SEARCHDIR].curWindowX, tex->buttons[BUT_SEARCHDIR].curWindowY},
        { tex->buttons[BUT_SEARCHDIR].tex->width + tex->buttons[BUT_SEARCHDIR].curWindowX,
          tex->buttons[BUT_SEARCHDIR].tex->height + tex->buttons[BUT_SEARCHDIR].curWindowY, }
    };

    if ( (event->type == SDL_KEYDOWN && event->type == SDLK_RETURN) ) printf("A");


    if ((event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT && GHP_clickIn(event->button.x, event->button.y, pos)) ||
        GHP_enterPressed(event)
    ) {

        char fileName[GHP_TEXT_LIMIT+4] = "games/";
        strcat(fileName,tex->texts[TEXT_SEARCHFILESENTRY].text);

        if ( strcmp(".txt", fileName + strlen(fileName) - 4 ) == 0 ) {
            game->logFile = fopen(fileName, "rt");
            if (!game->logFile)
                printf("\nFile %s not found.", fileName); // TODO: Show it to user
            else
                *mode = MODE_REPLAY;
        }

        if ( strcmp(".dat", fileName + strlen(fileName) - 4 ) == 0 ) {
            game->binFile = fopen(fileName, "r+b");
            if (!game->binFile)
                printf("\nFile %s not found.", fileName); // TODO: Show it to user
            else
                *mode = MODE_PLAY;
        }

    }



}

void renderSearchFile(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, int* mode) {
    GHP_setBGColor(renderer, 0, 0, 255, 255);
    GHP_renderTexture(renderer, tex->texts[TEXT_SEARCHFILESENTRY].tex, 52, 253);
    GHP_renderButton(renderer, &tex->buttons[BUT_SEARCHDIR], (WIDTH-(961-597))/2 , HEIGHT*0.1);
    GHP_renderButton(renderer, &tex->buttons[BUT_MENU], WIDTH*0.3, HEIGHT*0.7);

    // white rectangle border
    SDL_Rect rect = {50, 250, WIDTH-50*2, 50};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &rect);
}

// Replay

void initReplay(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, ConfigData* configData, int* mode) {
    GHP_setBGColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    GHP_renderMesh(renderer, &(tex->active_mesh), false);
    GHP_renderButton(renderer, &tex->buttons[BUT_REPLAYACTION], 1000, (HEIGHT-(763-640))/2);

    bool headers_loaded = false;
    char log[MAX_LEN_LOG];

    while(!headers_loaded) {
        fgets(log, MAX_LEN_LOG, game->logFile);
        if (strstr(log, "DIMENSION")) sscanf(log, "DIMENSION:%dx%d\n", &game->rows, &game->columns);
        else if (strstr(log, "MINES")) sscanf(log, "MINES:%d\n", &game->bombsNum);
        else if (strstr(log, "SEED")) {
            sscanf(log, "SEED:%d\n", &game->seed);
            headers_loaded = true;
        }
    }
    emptyField(game->rows, game->columns, game->field);
    coverAll(game->rows, game->columns, game->field);

}

void handlerReplay(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, SDL_Event* event, int* mode) {

    // TODO: Add to see the name

    char eventLog[21]; //TODO Hard coded
    char type;
    int i, j;
    int year, mon, day, hour, min, sec;
    char format[] = "Event:%[^/]/Type:%c/PosMesh:(%d,%d)/Date:(%d-%d-%d-%d-%d-%d)";


    // TODO check if problems loading

    if ((event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT && GHP_clickInButton(event->button.x, event->button.y, &tex->buttons[BUT_REPLAYACTION]))
        || GHP_enterPressed(event)) {

        char log[MAX_LEN_LOG];
        fgets(log, MAX_LEN_LOG, game->logFile);

        if (sscanf(log, format, &eventLog, &type, &i, &j, &year, &mon, &day, &hour, &min, &sec) >= 1) {

            //"%s CLICK\nCOORDS:\nX:%d\nY:%d\nDay:%d/%d/%d\nat %02d:%02d:%02d",type=='R' ? "RIGHT" : "LEFT"

            updateShowLogTexts(renderer, tex, type, i, j, year, mon, day, hour, min, sec);

            if (strcmp(eventLog, "Click") == 0) {

                int pos[2] = {i, j};

                if (game->started) {

                    if (type == 'L') {
                        if (userRevealCeld(i, j, game) == CELD_BOMB) {
                            game->started = false;
                            fclose(game->logFile);
                        }
                    } else if (type == 'R') {
                        userAuxActionCeld(i, j, game);
                    }
                } else {
                    initField(game, pos);
                    userRevealCeld(i, j, game);
                    game->started = true;
                }

            }
            else if (strcmp(eventLog, "Win") == 0) {
                *mode = MODE_SEARCHDIR;
                game->started = false;
            }

        } else { // no more logs -> game ended
            *mode = MODE_SEARCHDIR;
            game->started = false;
        }

    }

}

void renderReplay(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex, int* mode) {
    GHP_setBGColor(renderer, 0, 0, 0, 255);
    renderMeshUpdated(renderer, game, tex, tex->active_mesh);

    if (game->started) {
        GHP_renderTexture(renderer, tex->texts[TEXT_SHOWLOG_L1].tex, WIDTH_SPACE_MESH_MINES + 50, 100);
        GHP_renderTexture(renderer, tex->texts[TEXT_SHOWLOG_L2].tex, WIDTH_SPACE_MESH_MINES + 50, 125);
        GHP_renderTexture(renderer, tex->texts[TEXT_SHOWLOG_L3].tex, WIDTH_SPACE_MESH_MINES + 50, 150);
    }

    GHP_renderButton(renderer, &tex->buttons[BUT_REPLAYACTION], 1000, (HEIGHT-(763-640))/2);
}



// Buttons Reactions
void setModePlay(void* gameData, int* mode) {SDL_StartTextInput(); *mode = MODE_PLAY;}
void setModeMenu(void* gameData, int* mode) {*mode = MODE_MENU;}
void setModeLost(void* gameData, int* mode) {*mode = MODE_LOST;}
void setModeNameplayer(void* gameData, int* mode) { *mode = MODE_NAMEPLAYER; }
void setModeSearchDir(void* gameData, int*mode) {*mode = MODE_SEARCHDIR; }

