/*
 * Screamverter by RepellantMold (2023)
 * This code is licensed under MIT-0.
 */
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int convertSample(FILE* S3M, FILE* STM, unsigned short pointer, unsigned char num);
int generateSample(FILE* STM);
unsigned int grabSampleData(FILE* S3M, FILE* STM, unsigned short pointer);
int convertSampleData(FILE* S3M, FILE* STM, unsigned int pointer, unsigned short size);

unsigned char stmSampHeader[32] = {
        /* sample title (ASCIIZ) */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        /* another 0 */
        0,
        /* instrument disk */
        0,
        /* internal segment */
        0, 0,
        /* sample length */
        0, 0,
        /* loop start */
        0, 0,
        /* loop end */
        0xFF, 0xFF,
        /* default volume */
        0,
        /* reserved */
        0,
        /* C2 speed */
        0x00, 0x21,
        /* reserved */
        0, 0, 0, 0,
        /* segment (12-bit pointer) */
        0, 0

};

unsigned char* instrumentPointers, dataPointers;

unsigned int
grabSampleData(FILE* S3M, FILE* STM, unsigned short pointer) {
    fseek(S3M, pointer, SEEK_SET);
    fseek(S3M, 13, SEEK_CUR);
    unsigned int parapointer;
    fread(&parapointer, sizeof(unsigned char), 3, S3M);
    return parapointer << 4;
}

int
convertSample(FILE* S3M, FILE* STM, unsigned short pointer, unsigned char num)
{
    unsigned int i = 0;
    unsigned char* s3mIns = (unsigned char*)malloc(80);

    if (s3mIns == NULL) {
        puts("Memory allocation failed.");
        return 1;
    }

    fseek(S3M, pointer, SEEK_SET);

    fread(s3mIns, sizeof(char), 80, S3M);

    if (s3mIns[0] == 1) {
        /* copy the sample file name */
        if (s3mIns[1] != 0)
            for (i = 0; i < 12; i++)
                stmSampHeader[i] = s3mIns[i+1];
        else if (s3mIns[48] != 0)
            for (i = 0; i < 12; i++)
                stmSampHeader[i] = s3mIns[i+48];

        /* handle lengths */
        stmSampHeader[16] = s3mIns[16], stmSampHeader[17] = s3mIns[17];

        /*
         * if it loops, set the loop lengths
         * but otherwise just leave it the default
         */
        if (s3mIns[31] & 1) {
            stmSampHeader[18] = s3mIns[20], stmSampHeader[19] = s3mIns[21];
            stmSampHeader[20] = s3mIns[24], stmSampHeader[21] = s3mIns[25];
        } else {
            stmSampHeader[18] = 0, stmSampHeader[19] = 0;
            stmSampHeader[20] = 0xFF, stmSampHeader[21] = 0xFF;
        }

        /* copy default volume */
        stmSampHeader[22] = s3mIns[28];

        /* copy sample rate */
        stmSampHeader[24] = s3mIns[32], stmSampHeader[25] = s3mIns[33];
    } else {
        for (i = 0; i < 12; i++)
            stmSampHeader[i] = s3mIns[i+48];

        stmSampHeader[16] = 0, stmSampHeader[17] = 0;
        stmSampHeader[18] = 0, stmSampHeader[19] = 0;
        stmSampHeader[20] = 0xFF, stmSampHeader[21] = 0xFF;
        stmSampHeader[24] = 0x00, stmSampHeader[25] = 0x21;
    }

    free(s3mIns);

    fwrite(stmSampHeader, sizeof(char), sizeof(stmSampHeader), STM);
    return 0;
}

int
generateSample(FILE* STM)
{
    /* clear the metadata */
    memset(stmSampHeader, 0, 19);
    memset(&stmSampHeader[20], 0xFF, 2);

    fwrite(stmSampHeader, sizeof(char), sizeof(stmSampHeader), STM);
    return 0;
}

int
convertSampleData(FILE* S3M, FILE* STM, unsigned int pointer, unsigned short size) {
    unsigned short l = 0, p = 0;

    unsigned char* sampleData = (unsigned char*)calloc(size, sizeof(char));

    if (sampleData == NULL) {
        puts("Memory allocation failed.");
        return 1;
    }

    for (l = 0; l < size; l++) {
        sampleData[l] += 0x80;
    }

    fwrite(sampleData, sizeof(char), size, STM);

    free(sampleData);

    unsigned int remainingBytes = (16 - (ftell(STM) % 16)) % 16;

    for (; p < remainingBytes; p++) {
        fputc(0, STM);
    }

    return 0;
}
