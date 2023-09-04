/*
 * Screamverter by RepellantMold (2023)
 * This code is licensed under MIT-0.

 * I made sure to test this with a multitude of compilers I have available.

 * Compiled on:
 * Bloodshed's Dev-C++ 5.11 (TDM-GCC 4.9.2) (in a Windows XP virtual machine),
 * Microsoft Quick C (MS-DOS via DOSBox-X),
 * Tiny C Compiler (Windows 11 64-bit),
 * TDM-GCC 10.3.0 (Windows 11 64-bit)

 * Return values:
 * 0: Success
 * 1: Too many/not enough arguments
 * 2: Failed to allocate memory
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

	/* I'm only able to write in bytes at a time,
	and I have to dynamically allocate memory! */
	char *s3mHeader;
	unsigned char s3minstheader[80];
	unsigned char stminstheader[32*31];
	char *stmHeader;
	char *s3mPat;
	char *stmPat;
	char *orderArray;
	short *patptrArray;
	short *instptrArray;
	int *parapointer;

	register unsigned char p, r, c, s, o, l = 0;

	unsigned char ordCnt;
	unsigned char patCnt;
	unsigned char insCnt;

	unsigned short trackerInfo = 0;
	
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
			return 2;
		}

		ordCnt = s3mHeader[32];
		insCnt = s3mHeader[34];
		patCnt = s3mHeader[36];

		/* Null terminated string */
		printf("Song title: %.28s\n", s3mHeader);

		stmHeader = (char*)calloc(1040, sizeof(char));
		if (stmHeader == NULL) {
			puts("Failed to allocate memory!");
			return 2;
		}

		/* Copy over the title string */
		memcpy(stmHeader, s3mHeader, 20);

		/* add a terminator if needed */
		if (stmHeader[19] != 0) {
			stmHeader[19] = 0;
		}

		/* "!Scream!" */
		stmHeader[20] = 0x21;
		stmHeader[21] = 0x53;
		stmHeader[22] = 0x63;
		stmHeader[23] = 0x72;
		stmHeader[24] = 0x65;
		stmHeader[25] = 0x61;
		stmHeader[26] = 0x6D;
		stmHeader[27] = 0x21;

		/* DOS EoF */
		stmHeader[28] = 0x1A;

		/* Module */
		stmHeader[29] = 0x02;

		/* Scream Tracker 2.21 */
		stmHeader[30] = 0x02;
		stmHeader[31] = 0x15;

		/* Initital tempo */
		stmHeader[32] = (s3mHeader[49] << 4) + ((s3mHeader[50] / 125) & 0x0F);

		/* Number of patterns */
		stmHeader[33] = patCnt;

		/* Global volume */
		stmHeader[34] = s3mHeader[48];

		orderArray = (char*)calloc(ordCnt, sizeof(char));
		if (orderArray == NULL) {
			puts("Failed to allocate memory.");
			return 2;
		}

		instptrArray = (short*)calloc(insCnt, sizeof(short));
		if (instptrArray == NULL) {
			puts("Failed to allocate memory.");
			return 2;
		}

		patptrArray = (short*)calloc(patCnt, sizeof(short));
		if (patptrArray == NULL) {
			puts("Failed to allocate memory.");
			return 2;
		}
		
		fread(orderArray, sizeof(char), ordCnt, inS3M);
		fread(instptrArray, sizeof(short), insCnt, inS3M);

		for (s = 0; s < insCnt; ++s) {
			instptrArray[p * 2] <<= 4;
		}

		fread(patptrArray, sizeof(short), patCnt, inS3M);

		for (p = 0; p < patCnt; ++p) {
			patptrArray[p * 2] <<= 4;
		}

		/* Scream Tracker 2 uses 99 to denote no pattern (I think) */
		for (o = 0; o < 128; ++o) {
			orderArray[o] = 99;
		}

		for (o = 0; o < ordCnt; ++o) {
			if ((unsigned char)orderArray[o] < 254) {
				orderArray[l] = orderArray[o];
				++l;
				stmHeader[976 + l] = orderArray[l];
			}
		}

		/*

		for (s = 0; s < 31; ++s) {
			if (s < insCnt) {
				fseek(inS3M, instptrArray[s], SEEK_SET);
				fread(s3minstheader, sizeof(char), 80, inS3M);

				stminstheader[0 * s] = s3minstheader[1];
				stminstheader[1 * s] = s3minstheader[2];
				stminstheader[2 * s] = s3minstheader[3];
				stminstheader[3 * s] = s3minstheader[4];
				stminstheader[4 * s] = s3minstheader[5];
				stminstheader[5 * s] = s3minstheader[6];
				stminstheader[6 * s] = s3minstheader[7];
				stminstheader[7 * s] = s3minstheader[8];
				stminstheader[8 * s] = s3minstheader[9];
				stminstheader[9 * s] = s3minstheader[10];
				stminstheader[10 * s] = s3minstheader[11];
				stminstheader[11 * s] = s3minstheader[12];
				stminstheader[12 * s] = 0;
				stminstheader[13 * s] = 0;
				stminstheader[14 * s] = 0;
				stminstheader[15 * s] = 0;
				stminstheader[16 * s] = s3minstheader[16];
				stminstheader[17 * s] = s3minstheader[17];
				if (s3minstheader[31] & 1) {
					stminstheader[18 * s] = s3minstheader[20];
					stminstheader[19 * s] = s3minstheader[21];
					stminstheader[20 * s] = s3minstheader[24];
					stminstheader[21 * s] = s3minstheader[25];
				} else {
					stminstheader[18 * s] = 0;
					stminstheader[19 * s] = 0;
					stminstheader[20 * s] = 0xFF;
					stminstheader[21 * s] = 0xFF;
				}
				stminstheader[22 * s] = s3minstheader[28];
				stminstheader[23 * s] = 0;
				stminstheader[24 * s] = s3minstheader[32];
				stminstheader[25 * s] = s3minstheader[33];
				stminstheader[26 * s] = 0;
				stminstheader[27 * s] = 0;
				stminstheader[28 * s] = 0;
				stminstheader[29 * s] = 0;
				stminstheader[30 * s] = 0;
				stminstheader[31 * s] = 0;
			} else {
				stminstheader[0 * s] = 0;
				stminstheader[1 * s] = 0;
				stminstheader[2 * s] = 0;
				stminstheader[3 * s] = 0;
				stminstheader[4 * s] = 0;
				stminstheader[5 * s] = 0;
				stminstheader[6 * s] = 0;
				stminstheader[7 * s] = 0;
				stminstheader[8 * s] = 0;
				stminstheader[9 * s] = 0;
				stminstheader[10 * s] = 0;
				stminstheader[11 * s] = 0;
				stminstheader[12 * s] = 0;
				stminstheader[13 * s] = 0;
				stminstheader[14 * s] = 0;
				stminstheader[15 * s] = 0;
				stminstheader[16 * s] = 0;
				stminstheader[17 * s] = 0;	
				stminstheader[18 * s] = 0;
				stminstheader[19 * s] = 0;
				stminstheader[20 * s] = 0xFF;
				stminstheader[21 * s] = 0xFF;
				stminstheader[22 * s] = 0;
				stminstheader[23 * s] = 0;
				stminstheader[24 * s] = 0;
				stminstheader[25 * s] = 0;
				stminstheader[26 * s] = 0;
				stminstheader[27 * s] = 0;
				stminstheader[28 * s] = 0;
				stminstheader[29 * s] = 0;
				stminstheader[30 * s] = 0;
				stminstheader[31 * s] = 0;
			}
		}

		memcpy(&stmHeader[48], stminstheader, (sizeof(char) * 32) * 31);

		*/

		printf("Orders (excluding pattern markers) found: %u\n", l);

		fwrite(stmHeader, 1040, sizeof(char), outSTM);

		free(stmHeader);
		free(s3mHeader);
		free(orderArray);
		free(patptrArray);
		free(instptrArray);

		fclose(inS3M);
		fclose(outSTM);
		return 0;
	} else if( argc > 3 ) {
		puts("Too many arguments.");
		return 1;
	} else {
		puts("Expected usage: screamverter <filename.s3m> <filename.stm>");
		return 1;
	}
}
