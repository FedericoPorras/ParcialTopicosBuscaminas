#ifndef BUSCAMINAS_H
#define BUSCAMINAS_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

// Definiciones de constantes
#define MAX_SIZE 32  //Define la dimensi�n m�xima permitida (tanto filas como columnas) para el tablero de juego.
#define MIN_SIZE 8  //Define la dimensi�n m�nima permitida para el tablero de juego.
#define WINDOW_WIDTH 800  //define el ancho inicial (en p�xeles) de la ventana del juego que ser� creada por SDL2.
#define WINDOW_HEIGHT 600  //define la altura inicial (en p�xeles) de la ventana de juego creada por SDL2.

// Definici�n del estado de la celda
typedef enum {
    COVERED, //Representa el estado inicial de una celda, donde a�n no ha sido revelada por el jugador.
    UNCOVERED, //Indica que el jugador ha revelado esta celda.
    FLAGGED //Representa el estado en el que el jugador ha colocado una bandera en la celda, sospechando que contiene una mina.
} CellState;

// Definici�n de la estructura de la celda
typedef struct {
    bool hasMine;           //Indica si la celda contiene una mina (true) o no (false).
    int adjacentMines;  // Almacena el n�mero de minas que son adyacentes a esta celda. Si la celda en s� contiene una mina, se le asigna un valor especial como -1 para distinguirla.
    CellState state;   //Almacena el estado actual de la celda (COVERED, UNCOVERED, o FLAGGED).
} Cell;

// Variables globales (declaraci�n)
SDL_Window* window;
SDL_Renderer* renderer;
Cell board[MAX_SIZE][MAX_SIZE];
int rows, cols, numMines;
bool gameOver;
bool gameWon;
FILE* logFile;

// Declaraciones de funciones
bool initSDL(); //Inicializa la biblioteca SDL2. Devuelve true si la inicializaci�n fue exitosa y false en caso de error.
void cleanupSDL();  //Libera todos los recursos utilizados por SDL2.
void drawNumber(int number, int x, int y, int cellWidth, int cellHeight);  //Dibuja un n�mero (del 1 al 8) en una posici�n espec�fica (x, y) dentro de una celda del tablero, utilizando l�neas simples para representar los d�gitos.
void initBoard(); //Inicializa la matriz del tablero (board). Establece que ninguna celda tiene mina (hasMine = false), el n�mero de minas adyacentes es cero (adjacentMines = 0), y todas las celdas est�n cubiertas (state = COVERED).
void placeMines();  //Coloca aleatoriamente un n�mero predefinido de minas (numMines) en celdas del tablero que actualmente no tienen minas.
void calculateAdjacentMines(); //Recorre cada celda del tablero y calcula el n�mero de minas adyacentes para aquellas celdas que no contienen una mina.
void revealEmptyCells(int r, int c); //mplementa la l�gica de revelaci�n recursiva para celdas vac�as (sin minas adyacentes). Cuando se revela una celda sin minas vecinas, esta funci�n descubre autom�ticamente sus celdas vecinas y contin�a recursivamente si esas vecinas tambi�n est�n vac�as.
void revealCell(int r, int c);  //escubre la celda en la fila r y columna c cuando el jugador hace clic izquierdo.
void toggleFlag(int r, int c);  //Marca o desmarca una celda en la fila r y columna c con una bandera cuando el jugador hace clic derecho.
bool checkWin(); //Verifica si el jugador ha ganado el juego. Esto se hace comprobando si todas las celdas que no contienen minas han sido descubiertas.Devuelve true si el jugador ha ganado y false en caso contrario.
void drawBoard(); // Dibuja el estado actual del tablero(celdas cubiertas, descubiertas (mostrando n�meros o minas), y las celdas marcadas con banderas).
void resetGame();  //Restablece el juego a su estado inicial.
bool readConfig(const char* filename); //Lee la configuraci�n del juego (como las dimensiones del tablero y el n�mero de minas) desde un archivo especificado por filename (en este caso, "buscaminas.conf").

#endif // BUSCAMINAS_H
