/*
 * Screamverter by RepellantMold (2023)
 * This code is licensed under MIT-0.
 *
 * The MIT-Zero License
 *
 * Copyright (c) 2023 RepellantMold
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* for Visual Studio to shut up about using
their unportable versions of the library. |:c */
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* external function declarations */
extern int convertheader_S3MtoSTM(FILE* S3M, FILE* STM);

extern void convertSample(FILE* S3M, FILE* STM);
extern void generateSample(FILE* STM);

extern int parsepattern(FILE* S3M, unsigned short location, unsigned char num);
extern int conv_s3mpat_to_stmpat(char *s3mpat, char *stmpat);

int
main(int argc, char *argv[])
{
    puts("Screamverter, a Scream Tracker 3 module to 2 downgrader"
         "by RepellantMold (2023)\n"
         "This code is licensed under MIT-0.\n");

    unsigned char insNum = 0, patNum = 0, ordNum = 0;

    
    if (argc == 3) {
        FILE* S3M = fopen(argv[1], "rb");
        FILE* STM = fopen(argv[2], "wb");
        convertheader_S3MtoSTM(S3M, STM);
        fclose(S3M);
        fclose(STM);
    } else if (argc > 3) {
        puts("Too many arguments");
        return 1;
    } else {
        puts("Expected usage: screamverter input.s3m output.stm");
    }
    return 0;
}
