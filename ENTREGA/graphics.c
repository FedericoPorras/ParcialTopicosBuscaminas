#include "graphics.h"

bool GHP_SetWindow(struct GHP_WindowData* windowData, char* name, Reaction react, Drawing draw, int width, int height, GameState* game, GHP_TexturesData* textures) {
    // Initialize SDL and calls the react function. It will has as arguments the renderer, GameState, GHP_TexturesData*, and a draw function

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

    SDL_SetRenderDrawBlendMode(windowData->renderer, SDL_BLENDMODE_BLEND); // transparence

    react(windowData->renderer, game, textures, draw); // all functionality given from the user
    SDL_Delay(100); // to wait a little bit before closing and avoid consuming too resources

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

GHP_Texture GHP_newTexture(SDL_Renderer* renderer, const char* path, int offsetX, int offsetY, int width, int height) {
    // Initialize a texture. offsets of the asset.

    SDL_Texture* sdl_tex = IMG_LoadTexture(renderer, path); // Don't worry, SDL use dynamic memory so it wont be deleted
    if (!sdl_tex) {
        printf("Error loading texture: %s\n", IMG_GetError());
        return (GHP_Texture){NULL, -1, -1, -1, -1};
    }
    return (GHP_Texture){sdl_tex, offsetX, offsetY, width, height};
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

void GHP_renderMesh(SDL_Renderer* renderer, GHP_Mesh* mesh) {
    for (int i=0; i<mesh->rows; i++) {
        for (int j=0; j<mesh->cols; j++) {
            GHP_renderTexture(renderer, mesh->txtr, mesh->offsetX + i * mesh->txtr->width, mesh->offsetY + j * mesh->txtr->height);
        }
    }
}

void GHP_setBGColor(SDL_Renderer* renderer, int r, int g, int b, int a) { // TODO: Check if the present is actually neccesary
    // BE CAREFUL! It presents the render, so it has to be called before any render change
    SDL_SetRenderDrawColor(renderer, r,g,b,a);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

bool GHP_clickIn(int x, int y, int pos[2][2]) {
    return (pos[0][0] <= x && x <= pos[1][0]) && (pos[0][1] <= y && y <= pos [1][1]);
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
    int dim_pos[2][2] = {{*ini_pos, *(ini_pos+1)}, {*end_pos, *(end_pos+1)}}; // TODO: UGLY!
    return GHP_clickIn(x, y, dim_pos);
}

void GHP_coordsToPos(GHP_Mesh* mesh, int x, int y, int* pos) { // maybe switch the name to posToCoords
    *pos = (y-mesh->offsetY) / mesh->txtr->height;
    *(pos+1) = (x-mesh->offsetX) / mesh->txtr->width;
}

void GHP_posToCoords(GHP_Mesh* mesh, int x, int y, int* pos) {
    *pos = mesh->offsetX + mesh->txtr->width * x;
    *(pos+1) = mesh->offsetY + mesh->txtr->height * y;
}
