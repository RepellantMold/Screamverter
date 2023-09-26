/*
 * Screamverter by RepellantMold (2023)
 * This code is licensed under MIT-0.
 *
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

/* function prototypes (main) */
void generate16BytePadding(FILE *outSTM);

/* function prototypes (pattern) */
extern int readS3MPattern(FILE *inS3M, unsigned short pointer);
extern int convertPattern(FILE* outSTM);
extern unsigned char* s3mPat;
extern unsigned short patptrArray[255];
extern unsigned char channelRemap[32];

/* function prototypes (sample) */
extern void convertSample(FILE* inS3M, unsigned short parapointer, unsigned char pptaboffs);
extern void generateBlankSample();
extern int convertSampleData(FILE* inS3M, FILE* outSTM, unsigned int instdatptr, unsigned short size);
extern unsigned int crc32(unsigned char buf[], unsigned int buf_len);
extern unsigned int xorshift32(register unsigned int state);
extern unsigned short instptrArray[99];
extern unsigned int instdatptrArray[99];
extern unsigned char stmSampHeader[32];
extern unsigned short savedsamplelengths[99];

/* actual main function */
int main(int argc, char *argv[]) {
	unsigned char *s3mHeader;

	unsigned int afterposition;
	unsigned int beforeposition;

	/* having this be dynamic cause there's no way to tell what the size would be beforehand. */
	unsigned char *orderArray;

	/* some one lettererererers */
	register unsigned char p = 0, s = 0, o = 0, l = 0, c = 0;

	unsigned char channelread[32] = {
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255
	};

	/* counters */
	register unsigned char ordCnt = 0;
	register unsigned char patCnt = 0;
	register unsigned char insCnt = 0;

	unsigned char padding = 0;

	/* oh hey it's me! */
	puts("Screamverter by RepellantMold (2023)\n"
	"This code is licensed under MIT-0.\n"
	"This (within reason) converts S3M files to STM files.");

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
		
		s3mHeader = (unsigned char*)malloc(64);
		if (s3mHeader == NULL) {
			puts("Failed to allocate memory.");
			return 2;
		}

		/* read header */
		fread(s3mHeader, sizeof(char), 64, inS3M);

		fread(&channelread, sizeof(char), 32, inS3M);

		for (l = 0; l < 32; ++l) {
				channelRemap[l] = (channelread[l] < 16) ? c++ : 255;
		}

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
		
		free(s3mHeader);

		fwrite(stmSongHeader, sizeof(char), sizeof(stmSongHeader), outSTM);

		orderArray = (unsigned char*)calloc(ordCnt, sizeof(char));
		if (orderArray == NULL) {
			puts("Failed to allocate memory.");
			return 2;
		}

		fread(orderArray, sizeof(char), ordCnt, inS3M);
		fread(instptrArray, sizeof(char), insCnt * 2, inS3M);
		fread(patptrArray, sizeof(char), patCnt * 2, inS3M);

		for (s = 0; s < insCnt; ++s) {
			/* turn the parapointers into regular pointers */
			instptrArray[s] <<= 4;
		}

		for (p = 0; p < patCnt; ++p) {
			/* turn the parapointers into regular pointers */
			patptrArray[p] <<= 4;
		}

		if (ordCnt > 127) puts("WARNING: more than 128 orders found!");
		else printf("Found %d orders.\n", ordCnt);

		if (patCnt > 98) puts("WARNING: more than 99 patterns found!");
		else printf("Found %d patterns.\n", patCnt);

		if (insCnt > 30) puts("WARNING: more than 31 samples found!");
		else printf("Found %d samples.\n", insCnt);

		for (s = 0; s < 30; ++s) {
			/* convert all the instruments specified in the file, otherwise just generate a blank sample. */
			if (s < insCnt) convertSample(inS3M, instptrArray[s], s);
			else generateBlankSample();

			/* write it into the file */
			fwrite(stmSampHeader, sizeof(char), sizeof(stmSampHeader), outSTM);
		}

		/* add in the pattern markers */
		for (o = 0; o < ordCnt; ++o) {
			if (orderArray[o] < 99) {
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
			readS3MPattern(inS3M, patptrArray[p]);
			convertPattern(outSTM);
		}

		/* add padding */
		generate16BytePadding(outSTM);

		/* now grab the data */
		for (s = 0; s < 30; ++s) {
			/* generate the "paragraph" */
			unsigned char pointer[2] = { 0x00, 0x00 };
			beforeposition = (unsigned int)ftell(outSTM) >> 4;
			pointer[0] = (unsigned char)beforeposition;
			pointer[1] = (unsigned char)beforeposition >> 8;
			convertSampleData(inS3M, outSTM, instdatptrArray[s], savedsamplelengths[s]);
			afterposition = (unsigned int)ftell(outSTM);

			/* correct the pointer in the sample header that we couldn't get before. */
			/* printf("%04X\n", (pointer[1] << 8) + pointer[0]); */
			fseek(outSTM, sizeof(stmSongHeader) + ((sizeof(stmSampHeader) * s) - 2), SEEK_SET);
			fwrite(&pointer, sizeof(char), 2, outSTM);

			/* get back to our position before we had to correct the pointer. */
			fseek(outSTM, afterposition, SEEK_SET);
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

void generate16BytePadding(FILE *outSTM) {
	unsigned char paddingByte = 0;
	fwrite(&paddingByte, sizeof(char), (unsigned int)16 - (ftell(outSTM) % 16), outSTM);
}
