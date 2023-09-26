/*
 * Screamverter by RepellantMold (2023)
 * This code is licensed under MIT-0.
 */
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int convertheader_S3MtoSTM(FILE* S3M, FILE* STM);

unsigned char stmSongHeader[48] = {

    /* song title (ASCIIZ, 20 characters) */
    'S', 'r', 'e', 'a', 'm', 'v', 'e', 'r', 't', 'i', 'o', 'n', 0, 0, 0, 0, 0, 0, 0, 0,
    /* magic */
    '!', 'S', 'c', 'r', 'e', 'a', 'm', '!',
    /* DOS EoF */
    0x1A,
    /* file type (module) */
    2,
    /* Major/minor version (2.21) */
    2, 21,
    /* intial tempo */
    0x60,
    /* number of patterns */
    0x00,
    /* global volume */
    64,
    /* ??? */
    'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'

};

/* 99 (decimal) means no pattern (I think?...) */
unsigned char stmOrdTable[128] = {
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99
};

    /* define some external variables */
    extern unsigned char insNum = 0, patNum = 0, ordNum = 0;
    extern unsigned char* instrumentPointers, samplePointers;
    extern int convertSample(FILE* S3M, FILE* STM);
    extern int generateSample(FILE* STM);

int
convertheader_S3MtoSTM(FILE* S3M, FILE* STM) {
    unsigned char i = 0, p = 0, o = 0, a = 0;
    unsigned char* s3mHeader = (unsigned char*)malloc(96);

    if (s3mHeader == NULL) {
        puts("Memory allocation failed.");
        return 1;
    }

    fread(s3mHeader, sizeof(char), 96, S3M);

    ordNum = s3mHeader[0x20];
    insNum = s3mHeader[0x22];
    patNum = s3mHeader[0x24];

    /* Copy some of the metadata over */
    for (i = 0; i < 20; ++i) stmSongHeader[i] = s3mHeader[i];

    stmSongHeader[0x21] = patNum;
    stmSongHeader[0x22] = s3mHeader[0x30];
    stmSongHeader[0x20] = s3mHeader[0x31];

    fwrite(stmSongHeader, 1, sizeof(stmSongHeader), STM);

    /* free up memory */
    free(s3mHeader);

    /*
     * Handle orders (don't write them yet, it comes after the sample headers),
     * allocate the original order table
     */
    unsigned char* s3mOrdTable = (unsigned char*)malloc(ordNum);

    if (s3mOrdTable == NULL) {
        puts("Memory allocation failed.");
        return 1;
    }

    printf("%08X\n", ftell(S3M));

    /* Read the order table from the S3M pattern */
    fread(s3mOrdTable, sizeof(char), ordNum, S3M);

    /* Translate the list */
    a = 0;
    for (o = 0; o < ordNum; o++) {
        if (s3mOrdTable[o] <= 99) {
            stmOrdTable[a++] = s3mOrdTable[o];
        }
    }

    free(s3mOrdTable);

    printf("%02u patterns found with %02u orders\n", patNum, a);

    /* Do sample headers */
    for (i = 0; i < 30; ++i) {
        if (i < insNum) convertSample(S3M, STM);
        else generateSample(STM);
    }

    /* now write the order table */
    fwrite(stmOrdTable, sizeof(char), sizeof(stmOrdTable), STM);

    return 0;
}