//#include "buscaminas.h"
//#include <stdlib.h>
//#include <time.h>
//
//int main(int argc, char* argv[]) {
//    srand((unsigned int)time(NULL));
//    if (!initSDL()) {
//        return 1;
//    }
//
//    logFile = fopen("buscaminas.log", "a");
//    if (!logFile) {
//        printf("No se pudo abrir el archivo de log.\n");
//        return 1;
//    }
//
//    if (!readConfig("buscaminas.conf")) {
//        fclose(logFile);
//        return 1;
//    }
//
//    resetGame();
//
//    bool quit = false;
//    SDL_Event event;
//
//    while (!quit) {
//        while (SDL_PollEvent(&event)) {
//            if (event.type == SDL_QUIT) {
//                quit = true;
//            }
//            if (!gameOver && !gameWon) {
//                if (event.type == SDL_MOUSEBUTTONDOWN) {
//                    int x = event.button.x;
//                    int y = event.button.y;
//                    int r = (y * rows) / WINDOW_HEIGHT;
//                    int c = (x * cols) / WINDOW_WIDTH;
//
//                    if (r >= 0 && r < rows && c >= 0 && c < cols) {
//                        if (event.button.button == SDL_BUTTON_LEFT) {
//                            revealCell(r, c);
//                            if (gameOver) {
//                                 Revelar todas las minas
//                                for (int i = 0; i < rows; i++) {
//                                    for (int j = 0; j < cols; j++) {
//                                        if (board[i][j].hasMine) {
//                                            board[i][j].state = UNCOVERED;
//                                        }
//                                    }
//                                }
//                                SDL_Delay(500);
//                                quit = true;
//                                puts("Game over!");
//                            }
//                            if (!gameOver) {
//                                gameWon = checkWin();
//                                if(gameWon)
//                                {
//                                    SDL_Delay(500);
//                                    quit = true;
//                                    puts("winner!");
//                                }
//                            }
//                            fprintf(logFile, "Clic izquierdo en (%d, %d)\n", r, c);
//                        } else if (event.button.button == SDL_BUTTON_RIGHT) {
//                            toggleFlag(r, c);
//                            fprintf(logFile, "Clic derecho en (%d, %d)\n", r, c);
//                        }
//                    }
//                }
//            }
//        }
//
//        drawBoard();
//        SDL_RenderPresent(renderer);
//
//        SDL_Delay(16); // ~60 FPS
//    }
//
//    fclose(logFile);
//    cleanupSDL();
//    return 0;
//}
#include "buscaminas.h"
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]) {
    srand((unsigned int)time(NULL)); // Inicializa el generador de números aleatorios con la hora actual para obtener secuencias diferentes en cada ejecución.

    if (!initSDL()) { // Llama a la función para inicializar SDL .
        return 1;
    }

    logFile = fopen("buscaminas.log", "a"); // Abre un archivo llamado "buscaminas.log" en modo de "append" (añadir al final).
    if (!logFile) {
        printf("No se pudo abrir el archivo de log.\n");
        return 1;
    }

    if (!readConfig("buscaminas.conf")) { // Llama a la función para leer la configuración del juego desde el archivo "buscaminas.conf".
        fclose(logFile);                 // Si la lectura de la configuración falla, cierra el archivo de log (si se abrió).
        return 1;
    }

    resetGame(); // Llama a la función para inicializar el estado del juego (tablero, minas, etc.).

    bool quit = false;     // Variable de control para el bucle principal del juego. Se vuelve true para salir del juego.
    SDL_Event event;       // Estructura para almacenar los eventos de SDL (teclado, ratón, ventana, etc.).

    while (!quit) {         // Bucle principal del juego.
        while (SDL_PollEvent(&event)) { // Bucle para procesar todos los eventos pendientes de SDL.
            if (event.type == SDL_QUIT) { // Si el evento es de tipo SDL_QUIT (por ejemplo, cerrar la ventana).
                quit = true;              // Establece 'quit' a true para salir del bucle principal.
            }
            if (!gameOver && !gameWon) { // Comprueba si el juego no ha terminado (ni perdido ni ganado).
                if (event.type == SDL_MOUSEBUTTONDOWN) { // Si el evento es un botón del ratón presionado.
                    int x = event.button.x; // Obtiene la coordenada x del clic del ratón.
                    int y = event.button.y; // Obtiene la coordenada y del clic del ratón.
                    int r = (y * rows) / WINDOW_HEIGHT; // Calcula la fila del tablero en la que se hizo clic.
                    int c = (x * cols) / WINDOW_WIDTH;  // Calcula la columna del tablero en la que se hizo clic.

                    if (r >= 0 && r < rows && c >= 0 && c < cols) { // Comprueba si las coordenadas del clic están dentro de los límites del tablero.
                        if (event.button.button == SDL_BUTTON_LEFT) { // Si el botón presionado fue el botón izquierdo del ratón.
                            revealCell(r, c); // Llama a la función para revelar la celda en la fila 'r' y columna 'c'.
                            if (gameOver) {   // Comprueba si después de revelar la celda, el juego ha terminado (se pisó una mina).
                                // Revelar todas las minas
                                for (int i = 0; i < rows; i++) {
                                    for (int j = 0; j < cols; j++) {
                                        if (board[i][j].hasMine) {
                                            board[i][j].state = UNCOVERED; // Revela todas las celdas que contienen minas.
                                        }
                                    }
                                }
                                SDL_Delay(500); // Espera medio segundo para mostrar las minas antes de salir.
                                quit = true;    // Establece 'quit' a true para terminar el bucle principal.
                                puts("Game over!");
                            }
                            if (!gameOver) { // Si el juego no terminó después de revelar la celda.
                                gameWon = checkWin(); // Llama a la función para verificar si el jugador ha ganado.
                                if(gameWon) {         // Si la función 'checkWin' devuelve true (el jugador ha ganado).
                                    SDL_Delay(500); // Espera medio segundo antes de salir.
                                    quit = true;    // Establece 'quit' a true para terminar el bucle principal.
                                    puts("winner!");
                                    getchar();
                                }
                            }
                            fprintf(logFile, "Clic izquierdo en (%d, %d)\n", r, c); // Escribe en el archivo de log la acción del clic izquierdo.
                        } else if (event.button.button == SDL_BUTTON_RIGHT) { // Si el botón presionado fue el botón derecho del ratón.
                            toggleFlag(r, c); // Llama a la función para marcar o desmarcar una bandera en la celda.
                            fprintf(logFile, "Clic derecho en (%d, %d)\n", r, c); // Escribe en el archivo de log la acción del clic derecho.
                        }
                    }
                }
            }
        }

        drawBoard();           // Llama a la función para dibujar el estado actual del tablero en la ventana.
        SDL_RenderPresent(renderer); // Actualiza la pantalla con lo que se ha dibujado en el renderizador.

        SDL_Delay(16); // Introduce una pequeña pausa para limitar la velocidad de fotogramas (aproximadamente 60 FPS).
    }

    fclose(logFile);   // Cierra el archivo de log.
    cleanupSDL();    // Llama a la función para liberar los recursos de SDL.
    return 0;
}

