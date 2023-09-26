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

int
convertheader_S3MtoSTM(FILE* S3M, FILE* STM) {
    return 0;
}