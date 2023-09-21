#ifndef SCRATTERN
#define SCRATTERN
#include <stdio.h>
extern unsigned char channelRemap[32];
extern unsigned short patptrArray[255];
extern unsigned char s3mPatArray[64][16][5];
int readS3MPattern(FILE *inS3M, unsigned short pointer);
int convertPattern(FILE* outSTM);
#endif

