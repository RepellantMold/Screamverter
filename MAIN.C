/*
 * Screamverter by RepellantMold (2023)
 * This code is licensed under MIT-0.

 * Return values:
 * 0: Success
 * 1: Too many/not enough arguments
 * 2: Memory allocation error
 * -1: Invalid S3M file
 */

/* for Visual Studio to shut up about using
their unportable versions of the library. |:c */
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "pattern.h"
#include "sample.h"


int main(int argc, char *argv[]) {
	char s3mHeader[96];

	/*	(having this be dynamic cause
		there's no way to tell what
		the size would be beforehand.) */
	char *orderArray;


	/* some one lettererererers */
	register unsigned char p = 0, s = 0, o = 0, l = 0;

	/* counters */
	register unsigned char ordCnt = 0;
	register unsigned char patCnt = 0;
	register unsigned char insCnt = 0;

	/* oh hey it's me! */
	puts("Screamverter by RepellantMold (2023)\n"
	"This code is licensed under MIT-0.\n"
	"This is a very rough for fun project, so expect some bugs.");

	if( argc == 3 ) {
		/* open input/output files (and error if something goes wrong in any way) */
		FILE *inS3M;
		FILE *outSTM;
		inS3M = fopen(argv[1], "rb");
		if (inS3M == NULL) {
			puts("Failed to open the file.");
			return 1;
		}

		outSTM = fopen(argv[2], "wb");
		if (outSTM == NULL) {
			puts("Failed to write the file.");
			return 1;
		}

		/* read header */
		fread(s3mHeader, sizeof(char), 96, inS3M);

		puts("Converting header...");

		/* check for "SCRM" */
		if (s3mHeader[44] != 'S' || s3mHeader[45] != 'C' || s3mHeader[46] != 'R' || s3mHeader[47] != 'M') {
			puts("Not a valid S3M file.");
			return -1;
		}

		ordCnt = s3mHeader[32];
		insCnt = s3mHeader[34];
		patCnt = s3mHeader[36];

		/* Null terminated string */
		printf("Song title: %.28s\n", s3mHeader);

		/* Copy over the title string */
		memcpy(stmSongHeader, s3mHeader, 19);

		/* Initital tempo */
		stmSongHeader[32] = (s3mHeader[49] << 4) + ((s3mHeader[50] / 125) & 0x0F);

		/* Number of patterns */
		stmSongHeader[33] = patCnt;

		/* Global volume */
		stmSongHeader[34] = s3mHeader[48];

		fwrite(stmSongHeader, sizeof(char), sizeof(stmSongHeader), outSTM);

		orderArray = (char*)calloc(ordCnt, sizeof(char));
		if (orderArray == NULL) {
			puts("Failed to allocate memory.");
			return 2;
		}

		fread(orderArray, sizeof(char), ordCnt, inS3M);
		fread(instptrArray, sizeof(char), insCnt * 2, inS3M);

		for (s = 0; s < insCnt; ++s) {
			/* turn the parapointers into regular pointers */
			instptrArray[s] <<= 4;
		}

		fread(patptrArray, sizeof(char), patCnt * 2, inS3M);

		for (p = 0; p < patCnt; ++p) {
			/* turn the parapointers into regular pointers */
			patptrArray[p] <<= 4;
		}

		if (insCnt > 31) puts("WARNING: more than 31 samples found!");
		else printf("Found %d samples.\n", insCnt);

		for (s = 0; s < 31; ++s) {
			/* convert all the instruments specified in the file, otherwise just generate a blank sample. */
			if (s < insCnt) convertSample(inS3M, instptrArray[s], s);
			else generateSample();

			/* write it into the file */
			fwrite(stmSampHeader, sizeof(char), sizeof(stmSampHeader), outSTM);
		}

		/* add in the pattern markers */
		for (o = 0; o < ordCnt; ++o) {
			if ((unsigned)orderArray[o] < 254) {
				/* Copy over the order data if there's no markers */
				stmOrdTable[l] = orderArray[o];
				++l;
			}
		}

		printf("Orders (excluding pattern markers) found: %u\n", l);

		free(orderArray);

		fwrite(stmOrdTable, sizeof(char), sizeof(stmOrdTable), outSTM);

		for (p = 0; p < patCnt; ++p) {
			printf("pattern %02X\n", p);
			convertPattern(inS3M, outSTM, patptrArray[p]);
		}

		/* now grab the data */
		for (s = 0; s < 31; ++s) {
			unsigned int pointer = ftell(outSTM) >> 4;
			convertSampleData(inS3M, outSTM, instdatptrArray[s], savedsamplelengths[s]);
			unsigned int beforeposition = ftell(outSTM);

			/* correct the pointer in the sample header that we couldn't get before. */
			fseek(outSTM, sizeof(stmSongHeader) + (sizeof(stmSampHeader) - 2), SEEK_SET);
			fwrite(&pointer, sizeof(char), 2, outSTM);

			/* get back to our position before we had to correct the pointer. */
			fseek(outSTM, beforeposition, SEEK_SET);
		}

		fclose(inS3M);
		fclose(outSTM);

		puts("Done!");
		return 0;
	} else if( argc > 3 ) {
		puts("Too many arguments.");
		return 1;
	} else {
		puts("Expected usage: screamverter <filename.s3m> <filename.stm>");
		return 1;
	}
}
