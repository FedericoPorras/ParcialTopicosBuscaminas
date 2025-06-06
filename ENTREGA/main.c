#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "graphics/graphics.h"
#include "main.h"

void draw(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* TexData, int* mode);
void react(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* TexData, Drawing draw);

bool initGame(GameState* game);
void emptyField(int rows, int cols, mineCeld field[rows][cols]);
int randomBombs(int rows, int cols, mineCeld field[rows][cols], int ammount, int seed);
void printFieldConsole(int rows, int cols, mineCeld field[rows][cols]);
void draw_init(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex_data);
char check_position(int i, int j, int rows, int cols);
void calcAdjacency(int rows, int cols, mineCeld field[rows][cols]);
void draw_update_play(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex_data);
void initTexData(GHP_TexturesData* tex_data, SDL_Renderer* renderer);
void revealAll(int rows, int cols, mineCeld field[rows][cols]);
void revealAdjacencies(int i, int j , int rows, int cols, mineCeld field[rows][cols]);
bool checkNoAdjacencies(int i, int j, int rows, int cols, mineCeld field[rows][cols]);
bool checkWin(int rows, int cols, mineCeld field[rows][cols]);
bool initConfig(GameState* game);
void nullGame(GameState* game); // TODO: Use it, replace other aparitions
bool validConfig(GameState* game);
void resetConfig();
void printGame(GameState* game);

int main (int argc, char *argv[]) {

    GameState game;
    if (!initGame(&game)) {
        printf("\nError in game initalizing");
        return 1;
    }

    GHP_TexturesData tex_data;

    char* nameWindow = "Buscaminas";
    struct GHP_WindowData myWindow;
    if (GHP_SetWindow(&myWindow, nameWindow, react, draw, 1300, 700, &game, &tex_data)) { // it returns true at the end of the game
        GHP_DestroyWindow(&myWindow);
        free(tex_data.textures);
    }

    //srand(time(NULL));

    getchar(); // avoid closing the cmd

    return 0;
}

void react(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* TexData, Drawing draw) {

    SDL_Event event;

    int mode = MODE_INIT;

    while (mode != MODE_END) {
    while (SDL_PollEvent(&event)) {

        if (event.type == SDL_QUIT) { // window 'X' button
            mode=MODE_END;
            printf("\nQUIT BUTTON PRESSED\n");
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            printf("\nClick %d %d", event.button.x, event.button.y);

            if(GHP_clickInMesh(event.button.x, event.button.y, &(TexData->active_mesh))) {
                int pos[2];
                GHP_coordsToPos(&(TexData->active_mesh), event.button.x, event.button.y, pos);
                printf("\nClick %d %d", *pos, *(pos+1));

                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (!game->squareField10[*pos][*(pos+1)].bomb) {
                        game->squareField10[*pos][*(pos+1)].revealed = true;
                        revealAdjacencies(*pos, *(pos+1), INIT_DIM, INIT_DIM, game->squareField10);
                    } else {
                        game->squareField10[*pos][*(pos+1)].loose = true;
                        revealAll(INIT_DIM, INIT_DIM, game->squareField10);
                        if (checkWin(INIT_DIM, INIT_DIM, game->squareField10))
                            printf("\n\n\nWIN!");
                    }
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    if (game->squareField10[*pos][*(pos+1)].flag) game->squareField10[*pos][*(pos+1)].flag = false;
                    else game->squareField10[*pos][*(pos+1)].flag = true;
                }
            }
        }
        draw(renderer, game, TexData, &mode);
    }
    }

}

void draw(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* TexData, int* mode) {
    switch(*mode) {
        case MODE_INIT:
            draw_init(renderer, game, TexData);
            if (!TexData->textures) {
                *mode = MODE_END;
            }
            SDL_RenderPresent(renderer);
            *mode = MODE_PLAY;
            break;
        case MODE_PLAY:
            draw_update_play(renderer, game, TexData);
            SDL_RenderPresent(renderer);
            break;
    }
}

bool initGame(GameState* game) {
    if (!initConfig(game)) {
        resetConfig();
        initConfig(game);
    }
    printGame(game);

    game->field = (mineCeld[game->rows][game->columns]) malloc(sizeof(mineCeld)*game->rows*game->columns);
    if (!game->field) {
        printf("\nMalloc Error. There is no memory for the field.");
        return 0;
    }

    emptyField(INIT_DIM, INIT_DIM, game->squareField10);
    randomBombs(INIT_DIM, INIT_DIM, game->squareField10, (INIT_DIM*INIT_DIM) * 0.15, -1);
    calcAdjacency(INIT_DIM, INIT_DIM, game->squareField10);
    printFieldConsole(INIT_DIM, INIT_DIM, game->squareField10);

    return 1;
}

void emptyField(int rows, int cols, mineCeld field[rows][cols]) {
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            field[i][j] = (mineCeld){false, false, false, false, 0};
        }
    }
}

int randomBombs(int rows, int cols, mineCeld field[rows][cols], int ammount, int seed) {
    // Load a seed
    int thisSeed;
    if (seed == -1) {
        thisSeed = time(NULL);
        srand(thisSeed);
    }
    else srand(seed); // or random it

    // Put the bombs
    int i=0;
    while(i<ammount) {
        mineCeld* random = &(field[rand()%rows][rand()%cols]);
        if (!random->bomb) {
            random->bomb = true;
            i++;
        }
    }

    if (seed == -1) return thisSeed;
    else return seed;
}

char check_position(int i, int j, int rows, int cols) { // TODO: USE HEXA INSTEAD OF BINARY!
    // bits
    // 012
    // 3X4
    // 567
    char rv = 0x0;
    if (i > 0 && j > 0)           rv |= 0b00000001;
    if (i > 0)                    rv |= 0b00000010;
    if (i > 0 && j<cols-1)        rv |= 0b00000100;
    if (j > 0)                    rv |= 0b00001000;
    if (j < cols-1)               rv |= 0b00010000;
    if (i < rows-1 && j > 0)      rv |= 0b00100000;
    if (i < rows-1)               rv |= 0b01000000;
    if (i < rows-1 && j < cols-1) rv |= 0b10000000;
    return rv;
}

void calcAdjacency(int rows, int cols, mineCeld field[rows][cols]) {// TODO: USE HEXA INSTEAD OF BINARY!
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            if (field[i][j].bomb) field[i][j].adjacency = -1;
            else {
                char where_search = check_position(i, j, rows, cols);
                int adj=0;
                if (where_search & 0b00000001 && field[i-1][j-1].bomb) adj++;
                if (where_search & 0b00000010 && field[i-1][j]  .bomb) adj++;
                if (where_search & 0b00000100 && field[i-1][j+1].bomb) adj++;
                if (where_search & 0b00001000 && field[i]  [j-1].bomb) adj++;
                if (where_search & 0b00010000 && field[i]  [j+1].bomb) adj++;
                if (where_search & 0b00100000 && field[i+1][j-1].bomb) adj++;
                if (where_search & 0b01000000 && field[i+1][j]  .bomb) adj++;
                if (where_search & 0b10000000 && field[i+1][j+1].bomb) adj++;
                field[i][j].adjacency = adj;

            }
        }
    }
}

bool checkNoAdjacencies(int i, int j, int rows, int cols, mineCeld field[rows][cols]) {// TOO SIMILAR TO CALCADJACENCY! maybe put it together
            char where_search = check_position(i, j, rows, cols);
            int adj=0;
            if (where_search & 0b00000001 && field[i-1][j-1].bomb && !field[i-1][j-1].flag) adj++;
            if (where_search & 0b00000010 && field[i-1][j]  .bomb && !field[i-1][j]  .flag) adj++;
            if (where_search & 0b00000100 && field[i-1][j+1].bomb && !field[i-1][j+1].flag) adj++;
            if (where_search & 0b00001000 && field[i]  [j-1].bomb && !field[i]  [j-1].flag) adj++;
            if (where_search & 0b00010000 && field[i]  [j+1].bomb && !field[i]  [j+1].flag) adj++;
            if (where_search & 0b00100000 && field[i+1][j-1].bomb && !field[i+1][j-1].flag) adj++;
            if (where_search & 0b01000000 && field[i+1][j]  .bomb && !field[i+1][j]  .flag) adj++;
            if (where_search & 0b10000000 && field[i+1][j+1].bomb && !field[i+1][j+1].flag) adj++;

            if (!adj && !field[i][j].bomb  && !field[i][j].flag) return true; // delete '!field[i][j].bomb' condition
            else return false;
}

void revealAdjacencies(int i, int j , int rows, int cols, mineCeld field[rows][cols]) { // TODO: Put better this kind of things
    if (checkNoAdjacencies(i,j,rows,cols,field)) field[i][j].revealed = true;
    else {
        if (!field[i][j].bomb && !field[i][j].flag) field[i][j].revealed = true;
        return;
    }

    char cp = check_position(i, j, rows, cols);
    if (cp & 0b00000001 && !field[i-1][j-1].revealed) revealAdjacencies(i-1, j-1, rows, cols, field);
    if (cp & 0b00000010 && !field[i-1][j]  .revealed) revealAdjacencies(i-1, j  , rows, cols, field);
    if (cp & 0b00000100 && !field[i-1][j+1].revealed) revealAdjacencies(i-1, j+1, rows, cols, field);
    if (cp & 0b00001000 && !field[i]  [j-1].revealed) revealAdjacencies(i  , j-1, rows, cols, field);
    if (cp & 0b00010000 && !field[i]  [j+1].revealed) revealAdjacencies(i  , j+1, rows, cols, field);
    if (cp & 0b00100000 && !field[i+1][j-1].revealed) revealAdjacencies(i+1, j-1, rows, cols, field);
    if (cp & 0b01000000 && !field[i+1][j]  .revealed) revealAdjacencies(i+1, j  , rows, cols, field);
    if (cp & 0b10000000 && !field[i+1][j+1].revealed) revealAdjacencies(i+1, j+1, rows, cols, field);
}

// TODO: void autoFlag / When you click and the only option is to be a flag in a celd to fill it with the flag

void printFieldConsole(int rows, int cols, mineCeld field[rows][cols]) {
    for (int i=0; i<rows; i++) {
        printf("|");
        for (int j=0; j<cols; j++) {
            if (field[i][j].bomb)
                printf("X");
            else
                printf("%d", field[i][j].adjacency);
            //printf(field[i][j].bomb ? "X" : " ");
            printf("|");
        }
        printf("\n");
    }
}

void draw_init(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex_data) {
    initTexData(tex_data, renderer); // TODO: Error message / This line is here because you need the renderer

    if (!tex_data->textures) return;

    tex_data->active_mesh = (GHP_Mesh){50, 50, &(tex_data->textures[9]), INIT_DIM, INIT_DIM};
    GHP_renderMesh(renderer, &(tex_data->active_mesh));

    //void GHP_renderMesh(SDL_Renderer* renderer, GHP_Mesh* mesh, int rows, int cols)
}

void draw_update_play(SDL_Renderer* renderer, GameState* game, GHP_TexturesData* tex_data) {

    // REMEMBER: To print you put backwards the i and j. REVIEW WHY BECAUSE I DONT REMEMBER

    GHP_Mesh mesh = tex_data->active_mesh;
    GHP_Texture hidden = tex_data->textures[9];
    GHP_Texture flag = tex_data->textures[TEX_FLAG];

    for (int i=0; i<INIT_DIM; i++) {
        for (int j=0; j<INIT_DIM; j++) {
            if (!game->squareField10[i][j].revealed) {
                if (game->squareField10[i][j].flag) GHP_renderTexture(renderer, &flag, j*flag.width+mesh.offsetX, i*flag.height+mesh.offsetY);
                else GHP_renderTexture(renderer, &hidden, j*hidden.width+mesh.offsetX, i*hidden.height+mesh.offsetY);
            }
            else if (!game->squareField10[i][j].bomb)
            {
                GHP_Texture adj_tex = tex_data->textures[game->squareField10[i][j].adjacency];
                GHP_renderTexture(renderer, &adj_tex, j*adj_tex.width+mesh.offsetX, i*adj_tex.height+mesh.offsetY);
            } else if (!game->squareField10[i][j].loose) { // bomb that made the player end the game
                GHP_Texture adj_tex = tex_data->textures[TEX_BOMB]; // TODO: Change name
                GHP_renderTexture(renderer, &adj_tex, j*adj_tex.width+mesh.offsetX, i*adj_tex.height+mesh.offsetY);
            } else { // bomb loose
                GHP_Texture adj_tex = tex_data->textures[TEX_BOMBBOOM]; // TODO: Change name
                GHP_renderTexture(renderer, &adj_tex, j*adj_tex.width+mesh.offsetX, i*adj_tex.height+mesh.offsetY);
            }
        }
    }
}

void initTexData(GHP_TexturesData* tex_data, SDL_Renderer* renderer) {
    tex_data->textures = malloc(sizeof(GHP_Texture) * AMMOUNT_TEXTURES);
    char path[] = "./graphics/img/celds.png";
    if (tex_data->textures) {
        *tex_data->textures      = GHP_newTexture(renderer, path, 23, 0, CELD_DIM, CELD_DIM); // 0
        *(tex_data->textures+1)  = GHP_newTexture(renderer, path, 46, 0, CELD_DIM, CELD_DIM); // 1
        *(tex_data->textures+2)  = GHP_newTexture(renderer, path, 69, 0, CELD_DIM, CELD_DIM); // 2
        *(tex_data->textures+3)  = GHP_newTexture(renderer, path, 92, 0, CELD_DIM, CELD_DIM); // 3
        *(tex_data->textures+4)  = GHP_newTexture(renderer, path, 115, 0, CELD_DIM, CELD_DIM); // 4
        *(tex_data->textures+5)  = GHP_newTexture(renderer, path, 138, 0, CELD_DIM, CELD_DIM); // 5
        *(tex_data->textures+6)  = GHP_newTexture(renderer, path, 46, 23, CELD_DIM, CELD_DIM); // 6
        *(tex_data->textures+7)  = GHP_newTexture(renderer, path, 69, 23, CELD_DIM, CELD_DIM); // 7
        *(tex_data->textures+8)  = GHP_newTexture(renderer, path, 92, 23, CELD_DIM, CELD_DIM); // 8
        *(tex_data->textures+9)  = GHP_newTexture(renderer, path, 0, 0, CELD_DIM, CELD_DIM); // hidden
        *(tex_data->textures+10) = GHP_newTexture(renderer, path, 0, 23, CELD_DIM, CELD_DIM); // flag
        *(tex_data->textures+11) = GHP_newTexture(renderer, path, 23, 23, CELD_DIM, CELD_DIM); // bomb_normal
        *(tex_data->textures+12) = GHP_newTexture(renderer, path, 138, 23, CELD_DIM, CELD_DIM); // bomb_boom
    }
    else
        puts("Error taking memory to the textures.");
}

void revealAll(int rows, int cols, mineCeld field[rows][cols]) {
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            field[i][j].revealed = true;
        }
    }
}

bool checkWin(int rows, int cols, mineCeld field [rows][cols]) {
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            if ((!field[i][j].revealed && !field[i][j].bomb) || field[i][j].loose) return false; // TODO: CHANGE, loose will be analized in other function
        }
    }
    return true;
}

bool initConfig(GameState* game) {
    nullGame(game);
    FILE* Fconfig = fopen("buscaminas.conf", "rt");
    if (!Fconfig) {
        printf("\nError opening the configuration file. Default configuration will be used");
        return false;
    }
    char option[MAX_LEN_OPTION];
    float value;
    char lastChar;
    while (fscanf(Fconfig, "\n%[a-zA-Z]=%f%c", option, &value, &lastChar) > 0) { // TODO: Check max string tolerable / remember if fscanf reachs EOF returns -1
        if (strcmp(option, "columnas") == 0)
            game->columns=(int)value;
        else if (strcmp(option, "filas") == 0)
            game->rows=(int)value;
        else if (strcmp(option, "minas") == 0) {
            if (lastChar == '%') game->bombsPer = value; // TODO: Check this kind of casts as parses
            else game->bombsNum = (int)value;
        }
    }
    fclose(Fconfig);

    // "Minas found once"  This has to be here because in the loop you may don't know cols and rows
    if (game->bombsNum > 0) game->bombsPer = (float)(game->columns * game->rows) / game->bombsNum;
    else game->bombsNum = (int)((game->columns * game->rows) * (game->bombsPer / 100));

    if (validConfig(game)) return true;
    else {
        printf("\nError loading the configuration file. Default configuration will be used");
        return false;
    }

}

void nullGame(GameState* game) {
    *game = (GameState){-1, -1, -1, -1, NULL}; // TODO: IDK it raises a warning
}

bool validConfig(GameState* game) {
    if (
        (game->bombsNum < MIN_BOMBS || game->bombsNum > MAX_BOMBS || game->bombsNum > game->columns * game->rows) ||
        (game->bombsPer < MIN_PER || game->bombsPer > MAX_PER) ||
        (game->columns < MIN_COLS || game->columns > MAX_COLS) ||
        (game->rows < MIN_ROWS || game->rows > MAX_ROWS)
    )
        return false;
    else return true;
}

void resetConfig() { // TODO: Handle possible errors with files
    //remove("buscaminas.conf");
    FILE* defaultConfig = fopen("buscaminas.conf", "wt"); // do we need a remove before?
    char def[] = "columnas=10\nfilas=10\nminas=15%\n";
    fwrite(def, sizeof(def), 1, defaultConfig); // sizeof(def) or strlen ?
    fclose(defaultConfig);
}

void printGame(GameState* game) {
    printf("\nCols:%d \nRows:%d \nbombsNum:%d \nbombsPer:%f \n", game->columns, game->rows, game->bombsNum, game->bombsPer);
}

/*
typedef struct {
    int columns;
    int rows;
    int bombsNum;
    float bombsPer; // percentage
    mineCeld squareField10[10][10];
} GameState;
*/




