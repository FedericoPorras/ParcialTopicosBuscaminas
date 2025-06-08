#ifndef CONFIGS_H_INCLUDED
#define CONFIGS_H_INCLUDED

#include <stdbool.h>
#include <string.h>
#include "gamestate.h"

#define MAX_LEN_OPTION 51
#define MIN_BOMBS 1
#define MAX_BOMBS 1000
#define MIN_PER 0.01
#define MAX_PER 100
#define MIN_COLS 8
#define MAX_COLS 40
#define MIN_ROWS 8
#define MAX_ROWS 40

typedef struct {
    int cols;
    int rows;
    int seed;
    int minesNum;
    float minesPer;
} ConfigData;

bool initConfig(ConfigData* game);
bool validConfig(ConfigData* game);
void applyConfig(ConfigData* configs, GameState* game);
void nullConfig(ConfigData* configs);
bool resetConfig();

#endif // CONFIGS_H_INCLUDED
