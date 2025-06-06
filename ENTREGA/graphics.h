#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#define SDL_MAIN_HANDLED // avoid SDLmain

#define MODE_INIT 0
#define MODE_PLAY 1
#define MODE_END -1

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../main.h"

struct GHP_WindowData {
    SDL_Window *window;
    SDL_Renderer *renderer;
};

typedef struct {
    SDL_Texture* tex;
    int offsetX; // in the asset
    int offsetY; // in the asset
    int width;
    int height;
} GHP_Texture;

typedef struct {
    int offsetX; // in the window
    int offsetY; // in the window
    GHP_Texture* txtr;
    int rows;
    int cols;
} GHP_Mesh;

typedef struct {
    GHP_Texture* textures;
    GHP_Mesh active_mesh;
} GHP_TexturesData;

typedef void (*Drawing) (SDL_Renderer* renderer, GameState* game, GHP_TexturesData* TexData, int* mode);
typedef void (*Reaction)(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* TexData, Drawing draw);

bool GHP_SetWindow(struct GHP_WindowData* windowData, char* name, Reaction react, Drawing draw, int width, int height, GameState* game, GHP_TexturesData* textures);
void GHP_DestroyWindow(struct GHP_WindowData* windowData);
GHP_Texture GHP_newTexture(SDL_Renderer* renderer, const char* path, int offsetX, int offsetY, int width, int height);
void GHP_renderTexture(SDL_Renderer* renderer, GHP_Texture* ghp_tex, int offsetX, int offsetY);
void GHP_destroyTexture(GHP_Texture* ghp_tex);
void GHP_renderMesh(SDL_Renderer* renderer, GHP_Mesh* mesh);
void GHP_setBGColor(SDL_Renderer* renderer, int r, int g, int b, int a);
bool GHP_clickIn(int x, int y, int pos[2][2]);
void GHP_meshPos(GHP_Mesh* mesh, int pos[2][2]);
void GHP_calcEndPosMesh(GHP_Mesh* mesh, int* pos);
void GHP_coordsToPos(GHP_Mesh* mesh, int x, int y, int* pos);
void GHP_posToCoords(GHP_Mesh* mesh, int x, int y, int* pos);
bool GHP_clickInMesh(int x, int y, GHP_Mesh* mesh);

#endif // GRAPHICS_H_INCLUDED

