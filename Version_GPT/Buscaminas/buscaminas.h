#ifndef BUSCAMINAS_H
#define BUSCAMINAS_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

// Definiciones de constantes
#define MAX_SIZE 32  //Define la dimensión máxima permitida (tanto filas como columnas) para el tablero de juego.
#define MIN_SIZE 8  //Define la dimensión mínima permitida para el tablero de juego.
#define WINDOW_WIDTH 800  //define el ancho inicial (en píxeles) de la ventana del juego que será creada por SDL2.
#define WINDOW_HEIGHT 600  //define la altura inicial (en píxeles) de la ventana de juego creada por SDL2.

// Definición del estado de la celda
typedef enum {
    COVERED, //Representa el estado inicial de una celda, donde aún no ha sido revelada por el jugador.
    UNCOVERED, //Indica que el jugador ha revelado esta celda.
    FLAGGED //Representa el estado en el que el jugador ha colocado una bandera en la celda, sospechando que contiene una mina.
} CellState;

// Definición de la estructura de la celda
typedef struct {
    bool hasMine;           //Indica si la celda contiene una mina (true) o no (false).
    int adjacentMines;  // Almacena el número de minas que son adyacentes a esta celda. Si la celda en sí contiene una mina, se le asigna un valor especial como -1 para distinguirla.
    CellState state;   //Almacena el estado actual de la celda (COVERED, UNCOVERED, o FLAGGED).
} Cell;

// Variables globales (declaración)
SDL_Window* window;
SDL_Renderer* renderer;
Cell board[MAX_SIZE][MAX_SIZE];
int rows, cols, numMines;
bool gameOver;
bool gameWon;
FILE* logFile;

// Declaraciones de funciones
bool initSDL(); //Inicializa la biblioteca SDL2. Devuelve true si la inicialización fue exitosa y false en caso de error.
void cleanupSDL();  //Libera todos los recursos utilizados por SDL2.
void drawNumber(int number, int x, int y, int cellWidth, int cellHeight);  //Dibuja un número (del 1 al 8) en una posición específica (x, y) dentro de una celda del tablero, utilizando líneas simples para representar los dígitos.
void initBoard(); //Inicializa la matriz del tablero (board). Establece que ninguna celda tiene mina (hasMine = false), el número de minas adyacentes es cero (adjacentMines = 0), y todas las celdas están cubiertas (state = COVERED).
void placeMines();  //Coloca aleatoriamente un número predefinido de minas (numMines) en celdas del tablero que actualmente no tienen minas.
void calculateAdjacentMines(); //Recorre cada celda del tablero y calcula el número de minas adyacentes para aquellas celdas que no contienen una mina.
void revealEmptyCells(int r, int c); //mplementa la lógica de revelación recursiva para celdas vacías (sin minas adyacentes). Cuando se revela una celda sin minas vecinas, esta función descubre automáticamente sus celdas vecinas y continúa recursivamente si esas vecinas también están vacías.
void revealCell(int r, int c);  //escubre la celda en la fila r y columna c cuando el jugador hace clic izquierdo.
void toggleFlag(int r, int c);  //Marca o desmarca una celda en la fila r y columna c con una bandera cuando el jugador hace clic derecho.
bool checkWin(); //Verifica si el jugador ha ganado el juego. Esto se hace comprobando si todas las celdas que no contienen minas han sido descubiertas.Devuelve true si el jugador ha ganado y false en caso contrario.
void drawBoard(); // Dibuja el estado actual del tablero(celdas cubiertas, descubiertas (mostrando números o minas), y las celdas marcadas con banderas).
void resetGame();  //Restablece el juego a su estado inicial.
bool readConfig(const char* filename); //Lee la configuración del juego (como las dimensiones del tablero y el número de minas) desde un archivo especificado por filename (en este caso, "buscaminas.conf").

#endif // BUSCAMINAS_H
