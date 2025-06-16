#include "configs.h"

bool initConfig(ConfigData* configs) {
    nullConfig(configs);
    FILE* Fconfig = fopen("buscaminas.conf", "rt");
    if (!Fconfig) {
        printf("\nError opening the configuration file. Default configuration will be used");
        return false;
    }
    char option[MAX_LEN_OPTION];
    float value;
    char lastChar;
    while (fscanf(Fconfig, "\n%[a-zA-Z]=%f%c", option, &value, &lastChar) > 0) {
        if (strcmp(option, "columnas") == 0)
            configs->cols=(int)value;
        else if (strcmp(option, "filas") == 0)
            configs->rows=(int)value;
        else if (strcmp(option, "semilla") == 0)
            configs->seed=(int)value;
        else if (strcmp(option, "minas") == 0) {
            if (lastChar == '%') configs->minesPer = value;
            else configs->minesNum = (int)value;
        }
    }
    fclose(Fconfig);

    // This has to be here because in the loop you may don't know cols and rows
    if (configs->minesNum > 0) configs->minesPer = (float)(configs->cols * configs->rows) / configs->minesNum;
    else configs->minesNum = (int)((configs->cols * configs->rows) * (configs->minesPer / 100));

    if (validConfig(configs)) return true;
    else {
        printf("\nError loading the configuration file. Default configuration will be used");
        return false;
    }

}

bool validConfig(ConfigData* configs) {
    if (
        (configs->minesNum < MIN_BOMBS || configs->minesNum > MAX_BOMBS || configs->minesNum > configs->cols * configs->rows) ||
        (configs->minesPer < MIN_PER || configs->minesPer > MAX_PER) ||
        (configs->cols < MIN_COLS || configs->cols > MAX_COLS) ||
        (configs->rows < MIN_ROWS || configs->rows > MAX_ROWS) ||
        !configs->seed
    )
        return false;
    else return true;
}

bool resetConfig() {
    FILE* defaultConfig = fopen("buscaminas.conf", "wt");
    if (!defaultConfig) {
        printf("\nError opening default config file.");
        return false;
    }

    char def[] = "columnas=10\nfilas=10\nminas=15%\nsemilla=-1";
    fwrite(def, sizeof(def), 1, defaultConfig);
    fclose(defaultConfig);
    return true;
}

void nullConfig(ConfigData* configs) {
    configs->rows = -1; configs->cols = -1;
    configs->minesNum = -1;
    configs->seed = -1;
}

void applyConfig(ConfigData* configs, GameState* game) {
    game->rows = configs->rows;
    game->columns = configs->cols;
    game->seed = configs->seed;
    game->bombsNum = configs->minesNum;
}

