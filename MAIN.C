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
	char *s3minstheader;
	char *stmHeader;
	char *s3mPat;
	char *stmPat;
	char *orderArray;
	char *patptrArray;
	char *instptrArray;

	unsigned char p, r, c, s, o, l = 0;

	unsigned char orderLen;
	unsigned char numofPats;
	unsigned char numofinsts;

	unsigned char ordCnt;
	unsigned char insCnt;
	unsigned char patCnt;

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

		fread(s3mHeader, 96, 1, inS3M);

		puts("Converting header...");

		if (s3mHeader[44] != 'S' || s3mHeader[45] != 'C' || s3mHeader[46] != 'R' || s3mHeader[47] != 'M') {
			puts("Not a valid S3M file.");
			return 2;
		}

		ordCnt = s3mHeader[32];
		insCnt = s3mHeader[34];
		patCnt = s3mHeader[36];

		/* Little endian things... */
		trackerInfo = ((s3mHeader[41] << 8) + s3mHeader[40]);

		printf("Tracker info: %04X\n", trackerInfo);

		orderLen = s3mHeader[32];
		numofPats = s3mHeader[36];

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
		stmHeader[33] = s3mHeader[36];

		/* Global volume */
		stmHeader[34] = s3mHeader[48];

		orderArray = (char*)calloc(255, sizeof(char));
		if (orderArray == NULL) {
			puts("Failed to allocate memory for the STM header.");
			return 2;
		}
		
		fread(orderArray, sizeof(char), orderLen, inS3M);

		for (o = 0; o < orderLen; ++o) {
			if ((unsigned char)orderArray[o] < 254) {
				orderArray[l] = orderArray[o];
				++l;
				if ((unsigned char)orderArray[o] > numofPats)
					numofPats = (unsigned char)orderArray[o];
			}
		}

		printf("Orders (excluding pattern markers) found: %u\n", l);

		fwrite(stmHeader, 1040, sizeof(char), outSTM);

		free(stmHeader);
		free(s3mHeader);
		free(orderArray);

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
