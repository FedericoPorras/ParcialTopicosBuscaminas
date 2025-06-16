// aclarar espiritu

#include "graphics.h"

bool GHP_SetWindow(struct GHP_WindowData* windowData, char* name, Reaction react, int width, int height, void* gameData, GHP_TexturesData* textures) {
    // Initialize SDL and calls the react function. It will has as arguments the renderer, void* gameData for any info of the game, GHP_TexturesData*, and a draw function

    // init SDL
    if (SDL_Init(SDL_INIT_VIDEO)) {
        printf("\nError initializing SDL. SDL_Error: %s", SDL_GetError());
        return false;
    }

    // init window
    windowData->window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!windowData->window) {
        printf("\nError initializing the window. SDL_Error: %s", SDL_GetError());
        return false;
    }

    // init renderer
    windowData->renderer = SDL_CreateRenderer(windowData->window, -1, SDL_RENDERER_ACCELERATED);
    if (!windowData->renderer) {
        SDL_DestroyWindow(windowData->window);
        printf("\nError initializing the renderer. SDL_Error %s", SDL_GetError());
        return false;
    }

    // texts
    if (TTF_Init() == -1) {
        printf("\nError initializing SDL_ttf: %s\n", TTF_GetError());
        return false;
    }
    SDL_StartTextInput();

    SDL_SetRenderDrawBlendMode(windowData->renderer, SDL_BLENDMODE_BLEND); // transparence

    react(windowData->renderer, gameData, textures); // all functionality given from the user
    SDL_Delay(100); // to wait a little bit before closing and avoid consuming too resources

    SDL_StopTextInput();
    TTF_Quit();
    SDL_DestroyRenderer(windowData->renderer);
    SDL_DestroyWindow(windowData->window);
    SDL_Quit();

    return true;
}

void GHP_DestroyWindow(struct GHP_WindowData* windowData) {
    if (windowData->renderer) SDL_DestroyRenderer(windowData->renderer);
    if (windowData->window) SDL_DestroyWindow(windowData->window);
    SDL_Quit();
}

void GHP_DestroyTexturesData(GHP_TexturesData* data) {
    for (int i=0; i<data->texts_loaded; i++)
        GHP_destroyTexture(data->textsTexs + i);
    free(data->textsTexs);
    for (int i=0; i<data->buttons_loaded; i++)
        GHP_destroyTexture(data->buttons->tex + i);
    free(data->buttonsTexs);
    for (int i=0; i<data->textures_loaded; i++)
        GHP_destroyTexture(data->textures + i);
    free(data->textures);

}

GHP_Texture GHP_newTexture(SDL_Renderer* renderer, const char* path, int offsetX, int offsetY, int width, int height) {
    // Initialize a texture. offsets of the asset.

    SDL_Texture* sdl_tex = IMG_LoadTexture(renderer, path);
    if (!sdl_tex) {
        printf("\nError loading texture: %s\n", IMG_GetError());
        return (GHP_Texture){NULL, -1, -1, -1, -1};
    }

    return (GHP_Texture){sdl_tex, offsetX, offsetY, width, height};
}

GHP_Texture GHP_newTextureAbs(SDL_Renderer* renderer, const char* path, int initX, int initY, int endX, int endY) {
    return GHP_newTexture(renderer, path, initX, initY, endX-initX, endY-initY);
}

void GHP_renderTexture(SDL_Renderer* renderer, GHP_Texture* ghp_tex, int offsetX, int offsetY) {
    // Loads a texture on the renderer. offsets of the window (renderer)

    SDL_Rect ghp_tex_rect = {ghp_tex->offsetX, ghp_tex->offsetY, ghp_tex->width, ghp_tex->height};
    SDL_Rect window_rect = {offsetX, offsetY, ghp_tex->width, ghp_tex->height};

    SDL_RenderCopy(renderer, ghp_tex->tex, &ghp_tex_rect, &window_rect);
}

void GHP_destroyTexture(GHP_Texture* ghp_tex) {
    SDL_DestroyTexture(ghp_tex->tex);
}

void GHP_renderMesh(SDL_Renderer* renderer, GHP_Mesh* mesh, int dynamicPresent) {

    if (dynamicPresent == 0) {
        for (int i=0; i<mesh->rows; i++) {
            for (int j=0; j<mesh->cols; j++) {
                GHP_renderTexture(renderer, mesh->txtr, mesh->offsetX + j * mesh->txtr->width, mesh->offsetY + i * mesh->txtr->height);
                if (dynamicPresent) SDL_RenderPresent(renderer);
            }
        }
    }


    if (dynamicPresent == 1) {
        for (int i=0; i<mesh->rows; i++) {
            for (int j=0; j<mesh->cols; j++) {
                GHP_renderTexture(renderer, mesh->txtr, mesh->offsetX + j * mesh->txtr->width, mesh->offsetY + i * mesh->txtr->height);
                if (dynamicPresent) SDL_RenderPresent(renderer);
            }
        }
    }


    if (dynamicPresent == 2) {
        int rows = mesh->rows;
        int cols = mesh->cols;
        int totalCells = rows * cols;
        int cellsRendered = 0;

        int centerX = cols / 2;
        int centerY = rows / 2;
        int x = centerX, y = centerY;

        int directions[4][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
        int currentDir = 0;
        int stepSize = 1;
        int stepsTaken = 0;
        int stepCount = 0;

        GHP_renderTexture(renderer, mesh->txtr,
                         mesh->offsetX + x * mesh->txtr->width,
                         mesh->offsetY + y * mesh->txtr->height);
        cellsRendered++;

        while (cellsRendered < totalCells) {

            SDL_Delay((cellsRendered/totalCells)*30);

            x += directions[currentDir][0];
            y += directions[currentDir][1];
            stepsTaken++;

            if (x >= 0 && x < cols && y >= 0 && y < rows) {
                GHP_renderTexture(renderer, mesh->txtr,
                                mesh->offsetX + x * mesh->txtr->width,
                                mesh->offsetY + y * mesh->txtr->height);
                cellsRendered++;
                if (dynamicPresent) SDL_RenderPresent(renderer);
            }

            if (stepsTaken == stepSize) {
                currentDir = (currentDir + 1) % 4;
                stepsTaken = 0;
                stepCount++;

                if (stepCount % 2 == 0) {
                    stepSize++;
                }
            }
        }
    }

    if (dynamicPresent == 3) {
        int rows = mesh->rows;
        int cols = mesh->cols;
        int totalCells = rows * cols;
        int cellsRendered = 0;

        int centerX = cols / 2;
        int centerY = rows / 2;

        GHP_renderTexture(renderer, mesh->txtr,
                         mesh->offsetX + centerX * mesh->txtr->width,
                         mesh->offsetY + centerY * mesh->txtr->height);
        cellsRendered++;
        SDL_RenderPresent(renderer);

        for (int radius = 1; cellsRendered < totalCells; radius++) {
            for (int i = 0; i <= radius; i++) {
                int j = radius - i;

                int coords[4][2] = {
                    {centerX + i, centerY + j},
                    {centerX - i, centerY + j},
                    {centerX + i, centerY - j},
                    {centerX - i, centerY - j}
                };

                for (int k = 0; k < 4; k++) {
                    int x = coords[k][0];
                    int y = coords[k][1];

                    if (radius == 1 && k > 0 && x == centerX && y == centerY) continue;

                    if (x >= 0 && x < cols && y >= 0 && y < rows) {
                        GHP_renderTexture(renderer, mesh->txtr,
                                         mesh->offsetX + x * mesh->txtr->width,
                                         mesh->offsetY + y * mesh->txtr->height);
                        cellsRendered++;
                    }
                }
            }

            SDL_RenderPresent(renderer);

            int n = 40;
            float more = (float)cellsRendered/totalCells - 0.7;
            if (more > 0)  n+=(int)(more*more*1000);
            SDL_Delay(n);


        }
    }

}

void GHP_setBGColor(SDL_Renderer* renderer, int r, int g, int b, int a) {
    SDL_SetRenderDrawColor(renderer, r,g,b,a);
    SDL_RenderClear(renderer);
}

bool GHP_clickIn(int x, int y, int pos[2][2]) {
    return (pos[0][0] <= x && x <= pos[1][0]) && (pos[0][1] <= y && y <= pos [1][1]);
}

bool GHP_clickInButton (int x, int y, GHP_Button* button) {
    int pos[2][2]= {
        {button->curWindowX, button->curWindowY},
        {button->curWindowX + button->tex->width, button->curWindowY + button->tex->height}
    };
    return GHP_clickIn(x, y, pos);
}

void GHP_meshPos(GHP_Mesh* mesh, int pos[2][2]) {
    pos[0][0] = mesh->offsetX;
    pos[0][1] = mesh->offsetY;
    pos[1][0] = mesh->offsetX+mesh->txtr->width*mesh->cols;
    pos[1][1] = mesh->offsetY+mesh->txtr->height*mesh->rows;
}

void GHP_calcEndPosMesh(GHP_Mesh* mesh, int* pos) { // pos[2]
    *pos = mesh->offsetX + mesh->txtr->width * mesh->cols;
    *(pos+1) = mesh->offsetY + mesh->txtr->height * mesh->rows;
}

bool GHP_clickInMesh(int x, int y, GHP_Mesh* mesh) {
    int ini_pos[2] = {mesh->offsetX, mesh->offsetY};
    int end_pos[2]; GHP_calcEndPosMesh(mesh, end_pos);
    int dim_pos[2][2] = {{*ini_pos, *(ini_pos+1)}, {*end_pos, *(end_pos+1)}};
    bool clicked = GHP_clickIn(x, y, dim_pos);
    if (x<mesh->rows*mesh->txtr->width+mesh->offsetX && y<mesh->cols*mesh->txtr->height+mesh->offsetY)
        return clicked;
    else return false;
}

void GHP_coordsToPos(GHP_Mesh* mesh, int x, int y, int* pos) { // maybe switch the name to posToCoords
    *pos = (y-mesh->offsetY) / mesh->txtr->height;
    *(pos+1) = (x-mesh->offsetX) / mesh->txtr->width;
}

void GHP_posToCoords(GHP_Mesh* mesh, int x, int y, int* pos) {
    *pos = mesh->offsetX + mesh->txtr->width * x;
    *(pos+1) = mesh->offsetY + mesh->txtr->height * y;
}

int GHP_loadRectAsset(SDL_Renderer* renderer, const char* path, GHP_Texture** texturesAsset, int ammount_textures, int width_item, int height_item, int colsAsset) {
    *texturesAsset = (GHP_Texture*) malloc(sizeof(GHP_Texture) * ammount_textures);

    if (! (*texturesAsset) ) {
        printf("\nError loading (Rect) asset.");
        return MEM_ERR;
    } else {
        int r=0, c=0;

        while (r*colsAsset + c < ammount_textures) {

            *(*texturesAsset+r*colsAsset+c) = GHP_newTexture(renderer, path, c*width_item,  r*height_item, width_item, height_item);
            if (!(*texturesAsset+r*colsAsset+c)->tex) {
                return FILE_ERR;
            }

            c++;
            if (c == colsAsset) {
                c=0;
                r++;
            }
        }

    }
    return OK;
}

void GHP_newButtonAbs(SDL_Renderer* renderer, char* path, GHP_TexturesData* texData, GHP_Button* button, int initX, int initY, int endX, int endY, ButtonReaction func) {
    texData->buttonsTexs[texData->buttons_loaded] = GHP_newTextureAbs(renderer, path, initX, initY, endX, endY);
    button->tex = &(texData->buttonsTexs[texData->buttons_loaded]);

    if (!button->tex) {
        printf("\nError loading a button texture.");
        button->tex = NULL;
    }
    button->on_click = func;

    texData->buttons_loaded++;
}

void GHP_renderButton(SDL_Renderer* renderer, GHP_Button* button, int windowX, int windowY) {
    button->curWindowX = windowX; button->curWindowY = windowY;
    GHP_renderTexture(renderer, button->tex, windowX, windowY);
}

GHP_Texture GHP_textTexture(SDL_Renderer* renderer, char* pathFont, int sizeFont, SDL_Color color, char* text) {
    // when text = '\0' it has an error, but does not inform because all texts could be '\0' at first. User is supposed that user will handle it.

    TTF_Font* font = TTF_OpenFont(pathFont, sizeFont);
    if (!font) {
        printf("\nError loading text font.");
        return (GHP_Texture){NULL, -1, -1, -1, -1};
    }

    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    if (!surface && strcmp(text,"")!=0) {
        printf("\nT:[%s]", text);
        printf("\nError generating text surface.");
        TTF_CloseFont(font);
        return (GHP_Texture){NULL, -1, -1, -1, -1};
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!tex && strcmp(text,"")!=0) {
        printf("\nError generating text texture.");
        TTF_CloseFont(font);
        return (GHP_Texture){NULL, -1, -1, -1, -1};
    }

    int w, h;
    SDL_QueryTexture(tex, NULL, NULL, &w, &h); // get the real size

    TTF_CloseFont(font);

    return (GHP_Texture){tex, 0, 0, w, h};
}

void GHP_newText(SDL_Renderer* renderer, char* path, GHP_TexturesData* texData, GHP_Text* text, int windowX, int windowY, int sizeFont, SDL_Color color) {
    texData->textsTexs[texData->texts_loaded] = GHP_textTexture(renderer, path, sizeFont, color, text->text);
    text->tex = &(texData->textsTexs[texData->texts_loaded]);
    texData->texts_loaded++;

    if(!text->tex) {
        printf("\nError loading a text texture.");
        text->tex = NULL;
    }

    text->windowX = windowX;
    text->windowY = windowY;

    if (strlen(path) > 50) {
        printf("\nError generating text texture. Limit of 50 chars exceeded");
        text->tex = NULL;
    } else
        strcpy(text->path,path);
}

void GHP_updateTextTexture(SDL_Renderer* renderer, GHP_TexturesData* texData, int numberText, int sizeFont, SDL_Color color) {
    texData->textsTexs[numberText] = GHP_textTexture(renderer, texData->texts[numberText].path, sizeFont, color, texData->texts[numberText].text);
}

void GHP_renderText(SDL_Renderer* renderer, GHP_TexturesData* texData, int numberText, int sizeFont, SDL_Color color, int windowX, int windowY) {
    GHP_updateTextTexture(renderer, texData, numberText, sizeFont, color);
    GHP_renderTexture(renderer, texData->texts[numberText].tex, windowX, windowY);
}

bool GHP_enterPressed(SDL_Event* event) {
    return (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_RETURN);
}

bool GHP_setBG(SDL_Renderer* renderer, GHP_TexturesData* texData, char* path) {

    SDL_Texture* sdl_tex = IMG_LoadTexture(renderer, path);
    if (!sdl_tex) {
        printf("\nError loading background: %s\n", IMG_GetError());
        return false;
    }

    texData->background_tex = sdl_tex;
    return true;
}

void GHP_renderBG(SDL_Renderer* renderer, GHP_TexturesData* texData, int width, int height) {
    SDL_Rect ghp_tex_rect = {0, 0, width, height};
    SDL_Rect window_rect = {0, 0, width, height};
    SDL_RenderCopy(renderer, texData->background_tex, &ghp_tex_rect, &window_rect);
}

void GHP_freeBG(GHP_TexturesData* texData) {
    SDL_DestroyTexture(texData->background_tex);
}

void GHP_renderTextureExpress(SDL_Renderer* renderer, char* path, int imgInitX, int imgInitY, int imgEndX, int imgEndY, int winOffsetX, int winOffsetY) {
    GHP_Texture tex = GHP_newTextureAbs(renderer, path, imgInitX, imgInitY, imgEndX, imgEndY);
    GHP_renderTexture(renderer, &tex, winOffsetX, winOffsetY);
    GHP_destroyTexture(&tex);
}

