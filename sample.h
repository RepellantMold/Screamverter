#ifndef SCRAMPLE
#define SCRAMPLE
#include <stdio.h>

void convertSample(FILE* inS3M, unsigned short parapointer, unsigned char pptaboffs);
void generateSample();
int convertSampleData(FILE* inS3M, FILE* outSTM, unsigned int instdatptr, unsigned short size);

unsigned int crc32(unsigned char buf[], unsigned int buf_len);
unsigned int xorshift32(register unsigned int state);

extern unsigned short instptrArray[99];
extern unsigned short instdatptrArray[99];
extern unsigned char stmSampHeader[32];
extern unsigned short savedsamplelengths[99];
#endif