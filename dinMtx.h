#ifndef DINMTX_H_INCLUDED
#define DINMTX_H_INCLUDED

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

void** newDinMtx(int rows, int cols, int len);
void destroyDinMtx(int rows, int cols, int len, void** mtx);

#endif // DINMTX_H_INCLUDED
