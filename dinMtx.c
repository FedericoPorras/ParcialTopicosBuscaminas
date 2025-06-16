#include "dinMtx.h"

void** newDinMtx(int rows, int cols, int len) {
    void** rv = malloc(sizeof(void*) * rows);
    if (!rv) {
        printf("\nMalloc Error. There is no memory for the field.");
        return NULL;
    }
    for (int i=0; i<rows; i++) {
        *(rv+i) = malloc(len * cols);
        if (!*(rv+i)) {
            printf("\nMalloc Error. There is no memory for the field.");
            for (int j=0; j<i; j++)
                free(*(rv+j));
            free(rv);
            return NULL;
        }
    }
    return rv;
}

void destroyDinMtx(int rows, int cols, int len, void** mtx) {
    for (int i=0; i<rows; i++) {
        free(*(mtx+i));
    }
    free(mtx);
}

