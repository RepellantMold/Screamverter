/*
 * Screamverter by RepellantMold (2023)
 * This code is licensed under MIT-0.

 * Return values:
 * 0: Success
 * 1: Too many/not enough arguments
 * 2: Memory allocation error
 * -1: Invalid S3M file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MAIN.H"
#include "clowncommon/clowncommon.h"

unsigned int xorshift32(unsigned int state) {
	state ^= state << 13;
	state ^= state >> 17;
	state ^= state << 5;
	return state;
}

int main(int argc, char *argv[]) {

	/* I'm only able to write in bytes at a time,
	and I have to dynamically allocate memory! */
	char *s3mHeader;
	unsigned char s3minstheader[80];
	char *s3mPat;
	char *stmPat;
	char *orderArray;
	unsigned short patptrArray[255];
	unsigned short instptrArray[99];
	unsigned int parapointer;

	unsigned char p, r, c, s, o, l, n = 0;

	unsigned char ordCnt;
	unsigned char patCnt;
	unsigned char insCnt;

	unsigned short patSize = 0;

	puts("Screamverter\nby RepellantMold (2023)");

	if( argc == 3 ) {
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

		s3mHeader = (char*)calloc(96, sizeof(char));
		if (s3mHeader == NULL) {
			puts("Failed to allocate memory!");
			return 2;
		}

		fread(s3mHeader, sizeof(char), 96, inS3M);

		puts("Converting header...");

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

		free(s3mHeader);

		orderArray = (char*)calloc(ordCnt, sizeof(char));
		if (orderArray == NULL) {
			puts("Failed to allocate memory.");
			return 2;
		}
		
		fread(orderArray, sizeof(char), ordCnt, inS3M);
		fread(instptrArray, sizeof(short), insCnt, inS3M);

		for (s = 0; s < insCnt; ++s) {
			/* turn the parapointers into regular pointers */
			instptrArray[s] <<= 4;
		}

		fread(patptrArray, sizeof(short), patCnt, inS3M);

		for (p = 0; p < patCnt; ++p) {
			/* turn the parapointers into regular pointers */
			patptrArray[p] <<= 4;
		}

		/* Copy over the sample data */
		for (s = 0; s < 31; ++s) {

			if (s < insCnt) {
				fseek(inS3M, instptrArray[s], SEEK_SET);
				/* printf("%X - %lX\n", instptrArray[s], ftell(inS3M)); */
				fread(s3minstheader, sizeof(char), 80, inS3M);

				/* file name */
				memcpy(stmSampHeader, &s3minstheader[1], 12);
				
				/* if the loop flag is set... */
				if (s3minstheader[31] & 1) {
					/* loop start and loop end */
					stmSampHeader[18] = s3minstheader[20];
					stmSampHeader[19] = s3minstheader[21];
					stmSampHeader[20] = s3minstheader[24];
					stmSampHeader[21] = s3minstheader[25];
				} else {
					/* no loop */
					stmSampHeader[18] = 0;
					stmSampHeader[19] = 0;
					stmSampHeader[20] = 0xFF;
					stmSampHeader[21] = 0xFF;
				}
				
				/* sample length */
				stmSampHeader[16] = s3minstheader[16];
				stmSampHeader[17] = s3minstheader[17];
			
				/* default volume */
				stmSampHeader[22] = s3minstheader[28];
				
				/* c2 speed */
				stmSampHeader[24] = s3minstheader[32];
				stmSampHeader[25] = s3minstheader[33];

				stmSampHeader[22] = s3minstheader[28];
			} else {
				/* default stuff */
				
				for (n = 0; n < 12; ++n) {
					stmSampHeader[n] = CC_CLAMP(0x20, 0x7E, xorshift32(1337));
				}

				/* sample length */
				stmSampHeader[16] = 0;
				stmSampHeader[17] = 0;

				/* loop start and loop end */
				stmSampHeader[18] = 0;
				stmSampHeader[19] = 0;
				stmSampHeader[20] = 0xFF;
				stmSampHeader[21] = 0xFF;

				/* default volume */
				stmSampHeader[22] = 0;

				/* c2 speed */
				stmSampHeader[24] = 81;
				stmSampHeader[25] = 92;
			} 

			fwrite(stmSampHeader, sizeof(char), sizeof(stmSampHeader), outSTM);
		}

		for (o = 0; o < ordCnt; ++o) {
			if ((unsigned char)orderArray[o] < 254) {
				/* Copy over the order data if there's no markers */
				stmOrdTable[l] = orderArray[o];
				++l;
			}
		}

		free(orderArray);

		printf("Orders (excluding pattern markers) found: %u\n", l);

		fwrite(stmOrdTable, sizeof(char), sizeof(stmOrdTable), outSTM);

		for(p = 0; p < patCnt; ++p) {
			fseek(inS3M, patptrArray[p], SEEK_SET);
			fread(&patSize, sizeof(char), 2, inS3M);

			s3mPat = (char*)calloc(patSize, sizeof(char));
			if (s3mPat == NULL) {
				puts("Failed to allocate memory!");
				return 2;
			}

			free(s3mPat);
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
