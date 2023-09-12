#ifndef SCRATTERN
#define SCRATTERN
#include <stdio.h>

extern unsigned short patptrArray[255];

int convertPattern(FILE *inS3M, FILE* outSTM, unsigned short pointer);
#endif