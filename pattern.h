#ifndef SCRATTERN
#define SCRATTERN

unsigned char cv = 0;
unsigned short patSize = 0;
unsigned int stPat = 0xFF018000;
unsigned char s3mNote = 255, s3mIns = 0, s3mVol = 255, s3mEff = 0, s3mParam = 0;
unsigned short patptrArray[255];
char* s3mPat;

void convertPattern(FILE* inS3M, FILE* outSTM, unsigned short pointer);
#endif