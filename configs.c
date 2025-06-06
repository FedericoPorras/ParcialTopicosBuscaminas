#include "configs.h"

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
        else if (strcmp(option, "semilla") == 0)
            game->seed=(int)value;
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

bool validConfig(GameState* game) {
    if (
        (game->bombsNum < MIN_BOMBS || game->bombsNum > MAX_BOMBS || game->bombsNum > game->columns * game->rows) ||
        (game->bombsPer < MIN_PER || game->bombsPer > MAX_PER) ||
        (game->columns < MIN_COLS || game->columns > MAX_COLS) ||
        (game->rows < MIN_ROWS || game->rows > MAX_ROWS) ||
        !game->seed
    )
        return false;
    else return true;
}

void resetConfig() { // TODO: Handle possible errors with files
    //remove("buscaminas.conf");
    FILE* defaultConfig = fopen("buscaminas.conf", "wt"); // do we need a remove before?
    char def[] = "columnas=10\nfilas=10\nminas=15%\nsemilla=-1";
    fwrite(def, sizeof(def), 1, defaultConfig); // sizeof(def) or strlen ?
    fclose(defaultConfig);
}



