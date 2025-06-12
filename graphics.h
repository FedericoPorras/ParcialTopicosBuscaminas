#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#define SDL_MAIN_HANDLED // avoid SDLmain

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#define OK 0
#define MEM_ERR 1
#define DIM_ERR 2
#define FILE_ERR 3

#define GHP_TEXT_LIMIT 51
#define GHP_LIMIT_PATH 51

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
    GHP_Texture* tex;
    int curWindowX;
    int curWindowY;
    void (*on_click)(void* gameData, int* mode);
} GHP_Button;

typedef struct {
    GHP_Texture* tex;
    char path[GHP_LIMIT_PATH];
    int windowX;
    int windowY;
    char text[GHP_TEXT_LIMIT];
} GHP_Text;

typedef struct {
    GHP_Texture* textures;
    GHP_Button* buttons;
    GHP_Texture* buttonsTexs;
    GHP_Text* texts;
    GHP_Texture* textsTexs;
    int textures_loaded;
    int buttons_loaded;
    int texts_loaded;
    GHP_Mesh active_mesh;
} GHP_TexturesData;


//typedef void (*Drawing) (SDL_Renderer* renderer, void* gameData, GHP_TexturesData* TexData, int* mode);
typedef void (*Reaction)(SDL_Renderer* renderer, void* gameData, GHP_TexturesData* TexData);
typedef void (*ButtonReaction)(void* gameData, int* mode);

bool GHP_SetWindow(struct GHP_WindowData* windowData, char* name, Reaction react, int width, int height, void* game, GHP_TexturesData* textures);
void GHP_DestroyWindow(struct GHP_WindowData* windowData);
void GHP_DestroyTexturesData(GHP_TexturesData* data);
GHP_Texture GHP_newTexture(SDL_Renderer* renderer, const char* path, int offsetX, int offsetY, int width, int height);
GHP_Texture GHP_newTextureAbs(SDL_Renderer* renderer, const char* path, int initX, int initY, int endX, int endY);
void GHP_renderTexture(SDL_Renderer* renderer, GHP_Texture* ghp_tex, int offsetX, int offsetY);
void GHP_destroyTexture(GHP_Texture* ghp_tex);
void GHP_renderMesh(SDL_Renderer* renderer, GHP_Mesh* mesh, bool dynamicPresent);
void GHP_setBGColor(SDL_Renderer* renderer, int r, int g, int b, int a);
bool GHP_clickIn(int x, int y, int pos[2][2]);
void GHP_meshPos(GHP_Mesh* mesh, int pos[2][2]);
void GHP_calcEndPosMesh(GHP_Mesh* mesh, int* pos);
void GHP_coordsToPos(GHP_Mesh* mesh, int x, int y, int* pos);
void GHP_posToCoords(GHP_Mesh* mesh, int x, int y, int* pos);
bool GHP_clickInMesh(int x, int y, GHP_Mesh* mesh);
int GHP_loadRectAsset(SDL_Renderer* renderer, const char* path, GHP_Texture** texturesAsset, int ammount_textures, int width_item, int height_item, int colsAsset);
void GHP_newButtonAbs(SDL_Renderer* renderer, char* path, GHP_TexturesData* texData, GHP_Button* button, int initX, int initY, int endX, int endY, ButtonReaction func);
void GHP_renderButton(SDL_Renderer* renderer, GHP_Button* button, int windowX, int windowY);
GHP_Texture GHP_textTexture(SDL_Renderer* renderer, char* pathFont, int sizeFont, SDL_Color color, char* text);
void GHP_newText(SDL_Renderer* renderer, char* path, GHP_TexturesData* texData, GHP_Text* text, int windowX, int windowY, int sizeFont, SDL_Color color);
void GHP_updateTextTexture(SDL_Renderer* renderer, GHP_TexturesData* texData, int numberText, int sizeFont, SDL_Color color);
bool GHP_clickInButton (int x, int y, GHP_Button* button);
bool GHP_enterPressed(SDL_Event* event);

#endif // GRAPHICS_H_INCLUDED

