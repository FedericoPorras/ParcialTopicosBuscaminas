#include "buscaminas.h"
#include <stdlib.h>
#include <time.h>

// Variables globales (definición)
SDL_Window* window = NULL;  //Esta variable es un puntero que almacenará la dirección de la ventana principal .
                            // La ventana es la superficie donde se dibuja todo el contenido gráfico del juego (el tablero, las celdas, las minas, etc.).
                            //Se inicializa a NULL para indicar que aún no se ha creado ninguna ventana.

SDL_Renderer* renderer = NULL; //Esta variable es un puntero que almacenará la dirección del renderizador de SDL2.
                                //El renderizador es responsable de dibujar gráficos en la ventana.
                                // Proporciona funciones para dibujar puntos, líneas, rectángulos, texturas, etc.
                                //Se inicializa a NULL porque el renderizador se creará después de la ventana, utilizando la función SDL_CreateRenderer().

Cell board[MAX_SIZE][MAX_SIZE]; //Esta es la estructura de datos principal que representa el tablero de juego del Buscaminas.
                                // Cada elemento board[r][c] (donde r es la fila y c es la columna) es una estructura Cell que contiene información sobre esa celda en particular.

int rows, cols, numMines;
bool gameOver = false;
bool gameWon = false;
FILE* logFile;

// Función para inicializar SDL y el window y renderer
bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL no pudo inicializarse! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    window = SDL_CreateWindow("Buscaminas SDL2",
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                WINDOW_WIDTH,
                                WINDOW_HEIGHT,
                                SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("No se pudo crear la ventana! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("No se pudo crear el renderer! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

void cleanupSDL() {
    if(renderer) SDL_DestroyRenderer(renderer);
    if(window) SDL_DestroyWindow(window);
    SDL_Quit();
}

// Función para dibujar un número simple usando líneas
void drawNumber(int number, int x, int y, int cellWidth, int cellHeight) {
    if (number <= 0 || number > 8) return;

    // Configurar color según el número
    switch(number) {
        case 1: SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); break;     // Azul
        case 2: SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255); break;     // Verde
        case 3: SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); break;     // Rojo
        case 4: SDL_SetRenderDrawColor(renderer, 128, 0, 128, 255); break;   // Púrpura
        case 5: SDL_SetRenderDrawColor(renderer, 128, 0, 0, 255); break;     // Marrón
        case 6: SDL_SetRenderDrawColor(renderer, 0, 128, 128, 255); break;   // Turquesa
        case 7: SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); break;       // Negro
        case 8: SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255); break;    // Gris
    }

    // Calcular posición central y tamaño del número
    int centerX = x + cellWidth / 2;
    int centerY = y + cellHeight / 2;
    int size = (cellWidth < cellHeight ? cellWidth : cellHeight) / 3;

    // Dibujar números usando líneas simples
    switch(number) {
        case 1:
            SDL_RenderDrawLine(renderer, centerX, centerY - size, centerX, centerY + size);
            SDL_RenderDrawLine(renderer, centerX - size/3, centerY - size/2, centerX, centerY - size);
            break;

        case 2:
            SDL_RenderDrawLine(renderer, centerX - size/2, centerY - size, centerX + size/2, centerY - size);
            SDL_RenderDrawLine(renderer, centerX + size/2, centerY - size, centerX + size/2, centerY);
            SDL_RenderDrawLine(renderer, centerX + size/2, centerY, centerX - size/2, centerY);
            SDL_RenderDrawLine(renderer, centerX - size/2, centerY, centerX - size/2, centerY + size);
            SDL_RenderDrawLine(renderer, centerX - size/2, centerY + size, centerX + size/2, centerY + size);
            break;

        case 3:
            SDL_RenderDrawLine(renderer, centerX - size/2, centerY - size, centerX + size/2, centerY - size);
            SDL_RenderDrawLine(renderer, centerX + size/2, centerY - size, centerX + size/2, centerY);
            SDL_RenderDrawLine(renderer, centerX, centerY, centerX + size/2, centerY);
            SDL_RenderDrawLine(renderer, centerX + size/2, centerY, centerX + size/2, centerY + size);
            SDL_RenderDrawLine(renderer, centerX - size/2, centerY + size, centerX + size/2, centerY + size);
            break;

        case 4:
            SDL_RenderDrawLine(renderer, centerX - size/2, centerY - size, centerX - size/2, centerY);
            SDL_RenderDrawLine(renderer, centerX - size/2, centerY, centerX + size/2, centerY);
            SDL_RenderDrawLine(renderer, centerX + size/2, centerY - size, centerX + size/2, centerY + size);
            break;

        case 5:
            SDL_RenderDrawLine(renderer, centerX - size/2, centerY - size, centerX + size/2, centerY - size);
            SDL_RenderDrawLine(renderer, centerX - size/2, centerY - size, centerX - size/2, centerY);
            SDL_RenderDrawLine(renderer, centerX - size/2, centerY, centerX + size/2, centerY);
            SDL_RenderDrawLine(renderer, centerX + size/2, centerY, centerX + size/2, centerY + size);
            SDL_RenderDrawLine(renderer, centerX - size/2, centerY + size, centerX + size/2, centerY + size);
            break;

        case 6:
            SDL_RenderDrawLine(renderer, centerX - size/2, centerY - size, centerX + size/2, centerY - size);
            SDL_RenderDrawLine(renderer, centerX - size/2, centerY - size, centerX - size/2, centerY + size);
            SDL_RenderDrawLine(renderer, centerX - size/2, centerY, centerX + size/2, centerY);
            SDL_RenderDrawLine(renderer, centerX + size/2, centerY, centerX + size/2, centerY + size);
            SDL_RenderDrawLine(renderer, centerX - size/2, centerY + size, centerX + size/2, centerY + size);
            break;

        case 7:
            SDL_RenderDrawLine(renderer, centerX - size/2, centerY - size, centerX + size/2, centerY - size);
            SDL_RenderDrawLine(renderer, centerX + size/2, centerY - size, centerX + size/2, centerY + size);
            break;

        case 8:
            // Dibujar un rectángulo con línea horizontal en el medio
            SDL_RenderDrawLine(renderer, centerX - size/2, centerY - size, centerX + size/2, centerY - size);
            SDL_RenderDrawLine(renderer, centerX + size/2, centerY - size, centerX + size/2, centerY + size);
            SDL_RenderDrawLine(renderer, centerX + size/2, centerY + size, centerX - size/2, centerY + size);
            SDL_RenderDrawLine(renderer, centerX - size/2, centerY + size, centerX - size/2, centerY - size);
            SDL_RenderDrawLine(renderer, centerX - size/2, centerY, centerX + size/2, centerY);
            break;
    }
}

// Inicializa el tablero: limpia todo a cero, todas las celdas cubiertas y sin mina
void initBoard() {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            board[r][c].hasMine = false;
            board[r][c].adjacentMines = 0;
            board[r][c].state = COVERED;
        }
    }
}

// Coloca minas aleatoriamente en el tablero
void placeMines() {
    int placed = 0;
    while (placed < numMines) {
        int r = rand() % rows;
        int c = rand() % cols;
        if (!board[r][c].hasMine) {
            board[r][c].hasMine = true;
            placed++;
        }
    }
}

// Calcula para cada celda el número de minas adyacentes
void calculateAdjacentMines() {
    const int dr[] = {-1, -1, -1,  0, 0,  1, 1, 1};
    const int dc[] = {-1,  0,  1, -1, 1, -1, 0, 1};

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (board[r][c].hasMine) {
                board[r][c].adjacentMines = -1;
                continue;
            }
            int count = 0;
            for (int i = 0; i < 8; i++) {
                int nr = r + dr[i];
                int nc = c + dc[i];
                if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
                    if (board[nr][nc].hasMine) count++;
                }
            }
            board[r][c].adjacentMines = count;
        }
    }
}

// Desvela recursivamente todas las celdas vacías adyacentes (adjacentMines == 0)
void revealEmptyCells(int r, int c) {
    if (r < 0 || r >= rows || c < 0 || c >= cols) return;
    if (board[r][c].state == UNCOVERED || board[r][c].state == FLAGGED) return;

    board[r][c].state = UNCOVERED;
    if (board[r][c].adjacentMines == 0) {
        const int dr[] = {-1, -1, -1, 0, 0, 1, 1, 1};
        const int dc[] = {-1, 0, 1, -1, 1, -1, 0, 1};
        for (int i = 0; i < 8; i++) {
            revealEmptyCells(r + dr[i], c + dc[i]);
        }
    }
}

// Descubre la celda al hacer click izquierdo
void revealCell(int r, int c) {
    if (r < 0 || r >= rows || c < 0 || c >= cols) return;
    if (board[r][c].state == UNCOVERED || board[r][c].state == FLAGGED) return;

    // Si es mina, fin del juego
    if (board[r][c].hasMine) {
        board[r][c].state = UNCOVERED;
        gameOver = true;
        fprintf(logFile, "Fin del juego: Mina revelada en (%d, %d)\n", r, c);
        return;
    }

    // Si no tiene minas adyacentes, desvela recursivamente
    if (board[r][c].adjacentMines == 0) {
        revealEmptyCells(r, c);
    } else {
        board[r][c].state = UNCOVERED;
    }
}

// Marca o desmarca flag con click derecho
void toggleFlag(int r, int c) {
    if (r < 0 || r >= rows || c < 0 || c >= cols) return;
    if (board[r][c].state == UNCOVERED) return;
    if (board[r][c].state == COVERED) {
        board[r][c].state = FLAGGED;
        fprintf(logFile, "Marcado como mina en (%d, %d)\n", r, c);
    } else if (board[r][c].state == FLAGGED) {
        board[r][c].state = COVERED;
        fprintf(logFile, "Desmarcado en (%d, %d)\n", r, c);
    }
}

// Comprueba si el jugador ganó: todas las celdas que no son minas están descubiertas
bool checkWin() {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (!board[r][c].hasMine && board[r][c].state != UNCOVERED) {
                return false;
            }
        }
    }
    return true;
}

// Dibuja el tablero en pantalla
void drawBoard() {
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderClear(renderer);

    int cellWidth = WINDOW_WIDTH / cols;
    int cellHeight = WINDOW_HEIGHT / rows;

    // Dibujamos la zona del tablero
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int x = c * cellWidth;
            int y = r * cellHeight;

            // Fondo celda
            if (board[r][c].state == UNCOVERED) {
                SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
                SDL_Rect rect = {x, y, cellWidth, cellHeight};
                SDL_RenderFillRect(renderer, &rect);

                if (board[r][c].hasMine) {
                    // Dibuja mina como un círculo negro
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    int cx = x + cellWidth / 2;
                    int cy = y + cellHeight / 2;
                    int radius = cellWidth / 4;
                    for(int w = 0; w < radius * 2; w++) {
                        for(int h = 0; h < radius * 2; h++) {
                            int dx = radius - w;
                            int dy = radius - h;
                            if((dx*dx + dy*dy) <= (radius * radius)) {
                                SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);
                            }
                        }
                    }
                } else if (board[r][c].adjacentMines > 0) {
                    // Dibuja número de minas adyacentes
                    drawNumber(board[r][c].adjacentMines, x, y, cellWidth, cellHeight);
                }
            } else {
                // Celda cubierta o con bandera
                SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
                SDL_Rect rect = {x, y, cellWidth, cellHeight};
                SDL_RenderFillRect(renderer, &rect);

                if (board[r][c].state == FLAGGED) {
                    // Dibuja bandera (triángulo rojo)
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                    SDL_Point points[3] = {
                        {x + cellWidth / 4, y + cellHeight / 4},
                        {x + cellWidth / 4, y + 3 * cellHeight / 4},
                        {x + 3 * cellWidth / 4, y + cellHeight / 2}
                    };
                    SDL_RenderDrawLines(renderer, points, 3);
                    SDL_RenderDrawLine(renderer, points[2].x, points[2].y, points[0].x, points[0].y);
                }
            }

            // Dibuja borde celda
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
            SDL_Rect border = {x, y, cellWidth, cellHeight};
            SDL_RenderDrawRect(renderer, &border);
        }
    }
}

// Resetea el juego al estado inicial
void resetGame() {
    gameOver = false;
    gameWon = false;
    initBoard();
    placeMines();
    calculateAdjacentMines();
    fprintf(logFile, "Inicio del juego: %d x %d con %d minas\n", rows, cols, numMines);
}

// Lee la configuración desde el archivo
bool readConfig(const char* filename) {
    FILE* configFile = fopen(filename, "r");
    if (!configFile) {
        printf("No se pudo abrir el archivo de configuración: %s\n", filename);
        return false;
    }

    fscanf(configFile, "dimensiones=%dx%d\n", &rows, &cols);
    fscanf(configFile, "cantidad_minas=%d\n", &numMines);

    if (rows < MIN_SIZE || rows > MAX_SIZE || cols < MIN_SIZE || cols > MAX_SIZE || rows != cols) {
        printf("Dimensiones no válidas. Deben ser entre %d y %d y cuadradas.\n", MIN_SIZE, MAX_SIZE);
        fclose(configFile);
        return false;
    }

    if (numMines < 0 || (numMines > rows * cols)) {
        printf("Cantidad de minas no válida.\n");
        fclose(configFile);
        return false;
    }

    // Si se especifica como porcentaje
    if (numMines > 0 && numMines < 100) {
        numMines = (rows * cols) * numMines / 100;
    }

    fclose(configFile);
    return true;
}
