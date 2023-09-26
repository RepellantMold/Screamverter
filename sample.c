/*
 * Screamverter by RepellantMold (2023)
 * This code is licensed under MIT-0.
 */
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void generateSample()
{
    return;
}