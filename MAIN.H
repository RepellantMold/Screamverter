#ifndef SCREAM
#define SCREAM
#include <stdio.h>

unsigned char stmSongHeader[48] = {

	/* song title (ASCIIZ) */
	'S', 'r', 'e', 'a', 'm', 'v', 'e', 'r', 't', 'i', 'o', 'n', 0, 0, 0, 0,
	0, 0, 0, 0,
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
	99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
};

void generate16BytePadding(FILE *outSTM) {
	unsigned char paddingByte = 0;
	fwrite(&paddingByte, sizeof(char), 16 - (ftell(outSTM) % 16), outSTM);
};
#endif