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

#define _CRT_SECURE_NO_DEPRECATE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* FUNCTION DECLARATIONS */
int convertheader(FILE* S3M, FILE* STM, unsigned char* buffer);
int convertorders(FILE* S3M, FILE* STM);

void grabsomestuff(FILE* S3M);

void generatesampleheader(FILE* STM);
void parsepattern(FILE* S3M, unsigned int pointer, unsigned char* buffer);

void convertsampleheader(FILE* S3M, FILE* STM, unsigned char* buffer, unsigned int pointer, unsigned int id);
unsigned int convertsampledata(FILE* S3M, FILE* STM, unsigned char* buffer, unsigned int size, unsigned int pointer);
void converteffect(unsigned char eff, unsigned char prm);
void convertpattern(FILE* STM);

void showsongheader(unsigned char* buffer);
void showsampleheader(unsigned char* buffer);
unsigned int convertfromparapointer(const unsigned int parapointer);
unsigned int convert2parapointer(const unsigned int pointer);
unsigned int calculatepadding(const unsigned int size);

/* GLOBAL VARIABLES */
unsigned char orderCnt = 0, instCnt = 0, patCnt = 0, globalVol = 64;
unsigned char ticksPerRow = 6, ticksPerSec = 125;

unsigned short instrumentParapointers[99] = {0};
unsigned short patternParapointers[255] = {0};
unsigned int instrumentPCMOffsets[99] = {0};

unsigned char scrm[4] = {0};

unsigned char depackedS3Mpat[32][64][5] = {0};

unsigned char stmSongHeader[48] = {
	/* song title (C-style string, Code page 437) */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* magic */
	'!', 'S', 'c', 'r', 'e', 'a', 'm', '!',
	/* DOS EoF */
	0x1A,
	/* file type */
	0x02,
	/* Major/minor version */
	0x02, 0x15,
	/* intial tempo */
	0x60,
	/* number of patterns */
	0x00,
	/* global volume */
	0x40,
	/* more magic (identification for converter) */
	'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'
};

unsigned char stmSampHeader[32] = {
	/* sample title (C-style string, Code page 437) */
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
	64,
	/* reserved */
	0,
	/* middle C frequency */
	0, 0x21,
	/* reserved */
	0, 0, 0, 0,
	/* segment (12-bit pointer) */
	0, 0
};

/* ACTUAL FUNCTIONS */
int main(const int argc, const char *argv[])
{
	int i = 0, ret_value = 0;
	FILE *S3M, *STM;
	unsigned char *headerbuffer;

	/*printf("Hello World!\n");*/

	if(argc != 3) {
		printf("USAGE: %s input.s3m output.stm\n", argv[0]);
		ret_value = 1;
		goto retval;
	}

	S3M = fopen(argv[1], "rb");
	if (S3M == NULL) { goto foerr; }

	fseek(S3M, 44, SEEK_SET);
	fread(scrm, 1, 4, S3M);

	if (memcmp(scrm, "SCRM", 4) != 0) {
		fprintf(stderr, "This is not a Scream Tracker 3 module!\a");
		return 1;
	}

	rewind(S3M);

	STM = fopen(argv[2], "wb");
	if (STM == NULL) { goto foerr; }

	headerbuffer = (unsigned char*) malloc(64);
	if (headerbuffer == NULL) { goto allocerror; }

	if (0 != convertheader(S3M, STM, headerbuffer)) {
		free(headerbuffer);
		abort();
	}

	free(headerbuffer);

	grabsomestuff(S3M);

	for(; i < 31; i++) {
		if (i < instCnt) {
			headerbuffer = (unsigned char*) malloc(80);
			if (headerbuffer == NULL) { goto allocerror; }
			convertsampleheader(S3M, STM, headerbuffer, instrumentParapointers[i], i);
			free(headerbuffer);
		} else generatesampleheader(STM);
	}

	if (0 != convertorders(S3M, STM)) perror("\aerror with order conversion");

	fclose(S3M);
	fclose(STM);

	retval: return ret_value;

	foerr: perror("\afopen"); ret_value = 1; goto retval;
	allocerror: perror("\amalloc"); ret_value = 1; goto retval;
}

/* HEADER HANDLING */
int convertheader(FILE* S3M, FILE* STM, unsigned char* buffer) {
	fread(buffer, 1, 64, S3M);

	orderCnt = buffer[32];
	instCnt = buffer[34];
	patCnt = buffer[36];
	ticksPerRow = buffer[49];
	ticksPerSec = buffer[50];

	showsongheader(buffer);

	if (patCnt > 63) puts("WARNING:\tToo many patterns, data will be truncated!\a");
	if (instCnt > 30) puts("WARNING:\tToo many instruments, data will be truncated!\a");

	stmSongHeader[33] = patCnt;
	stmSongHeader[34] = buffer[30];

	/* copy the song title string */
	strncpy((char *)stmSongHeader, (char *)buffer, 20);

	fwrite(stmSongHeader, 1, sizeof(stmSongHeader), STM);

	return 0;
}

int convertorders(FILE* S3M, FILE* STM) {
	unsigned char *S3Morders, *STMorders;
	unsigned int i = 0, ret_value = 0;

	/* offset 96 is where the list is */
	fseek(S3M, 96, SEEK_SET);

	S3Morders = (unsigned char*) malloc(orderCnt);
	if (S3Morders == NULL) { goto allocerr; }

	fread(S3Morders, 1, orderCnt, S3M);

	STMorders = (unsigned char*) malloc(128);
	if (STMorders == NULL) { goto allocerr; }

	/* 9 = no pattern */
	memset(STMorders, 99, 128);

	for (; i < orderCnt; i++) {
		/*
		 * Scream Tracker 2 does not support patterns above 63,
		 * so limit the entries (conviently removes markers like 0xFE & 0xFF)
		 */
		STMorders[i] = (S3Morders[i] > 63) ? 63 : S3Morders[i];
	}

	free(S3Morders);

	fwrite(STMorders, 1, 128, STM);
	free(STMorders);

	returnvalue: return ret_value;

	allocerr: perror("malloc\a"); ret_value = 1; goto returnvalue;
}

void grabsomestuff(FILE* S3M) {
	unsigned int i = 0;
	fseek(S3M, 96 + orderCnt, SEEK_SET);
	fread(instrumentParapointers, 2, instCnt, S3M);
	fread(patternParapointers, 2, patCnt, S3M);

	for(i = 0; i < instCnt; i++) {
		instrumentParapointers[i] = convertfromparapointer(instrumentParapointers[i]);
	}

	for(i = 0; i < patCnt; i++) {
		patternParapointers[i] = convertfromparapointer(patternParapointers[i]);
	}

	return;
}

void generatesampleheader(FILE* STM) {
	/* basically clear everything to default values... */

	/* fill the file name string with NULL */
	memset(stmSampHeader, 0, 12);

	/* sample length (including loop start and end) */
	stmSampHeader[17] = 0x00, stmSampHeader[16] = 0x00;
	stmSampHeader[19] = 0x00, stmSampHeader[18] = 0x00;
	stmSampHeader[21] = 0xFF, stmSampHeader[20] = 0xFF;

	/* c frequency */
	stmSampHeader[25] = 0x21, stmSampHeader[24] = 0x00;

	/* make sure default volume is 0 */
	stmSampHeader[22] = 0x00;
	fwrite(stmSampHeader, 1, sizeof(stmSampHeader), STM);

	return;
}

void convertsampleheader(FILE* S3M, FILE* STM, unsigned char* buffer, unsigned int pointer, unsigned int id) {
	fseek(S3M, pointer, SEEK_SET);
	fread(buffer, 1, 80, S3M);

	showsampleheader(buffer);

	/* copy the sample name string (if it's not blank, though with heavy truncation) */
	if (buffer[48] != 0) strncpy((char *)&stmSampHeader[0], (char *)&buffer[48], 12);
	/* if it is blank, then just copy the file name field instead, which is the same length :P */
	else memcpy((char *)&stmSampHeader[0], (char *)&buffer[1], 12);

	/* handle different sample types */
	switch(buffer[0]) {
		/* adlib (both melody and drums) type */
		case 3:
		case 2:
		puts("WARNING:\tAdlib samples are not supported!\a");

		/* message type */
		case 0:
		/* sample length (including loop start and end) */
		stmSampHeader[17] = 0x00, stmSampHeader[16] = 0x00;
		stmSampHeader[19] = 0x00, stmSampHeader[18] = 0x00;
		stmSampHeader[21] = 0xFF, stmSampHeader[20] = 0xFF;

		/* c frequency */
		stmSampHeader[25] = 0x21, stmSampHeader[24] = 0x00;

		/* make sure default volume is 0 */
		stmSampHeader[22] = 0x00;
		break;

		/* sample type */
		case 1:
		/* set length */
		stmSampHeader[17] = buffer[17], stmSampHeader[16] = buffer[16];

		/* handle loop start and end depending if the loop flag is set */
		if((buffer[31] & 1) != 0) {
			stmSampHeader[19] = buffer[21], stmSampHeader[18] = buffer[20];
			stmSampHeader[21] = buffer[25], stmSampHeader[20] = buffer[24];
		} else {
			stmSampHeader[19] = 0x00, stmSampHeader[18] = 0x00;
			stmSampHeader[21] = 0xFF, stmSampHeader[20] = 0xFF;
		}

		/* c frequency */
		stmSampHeader[25] = buffer[33], stmSampHeader[24] = buffer[32];

		/* default volume */
		stmSampHeader[22] = buffer[28];
		break;
	}

	fwrite(stmSampHeader, 1, sizeof(stmSampHeader), STM);

	return;
}

/* SAMPLE HANDLING */
unsigned int convertsampledata(FILE* S3M, FILE* STM, unsigned char* buffer, unsigned int size, unsigned int pointer) {
	unsigned int i = 0;
	const unsigned int currentPos = ftell(S3M);
	const unsigned int padding = calculatepadding(currentPos);
	char *padbuffer;
	fseek(S3M, pointer, SEEK_SET);

	fread(buffer, 1, size, S3M);

	/* convert all the PCM values from unsigned to signed */
	for(; i < size; i++) {
		buffer[i] = (buffer[i] + 128) & 0xFF;
	}

	/* pad the file in 16 byte boundaries */
	if (padding > 0) {
		padbuffer = (char *)malloc(padding);
		memset(buffer, 0, padding);
		fwrite(buffer, 1, padding, STM);
		free(buffer);
	}

	return convert2parapointer(currentPos + padding);
}

/* PATTERN HANDLING */
void parsepattern(FILE* S3M, unsigned int pointer, unsigned char* buffer) {
	unsigned short buffsize = 0;
	unsigned char r = 0, c = 0, b = 0;

	fseek(S3M, pointer, SEEK_SET);
	fread((unsigned char *)&buffsize, 1, 2, S3M);

	fread(buffer, 1, buffsize, S3M);

	while(1) {
		b = *buffer++;
		if(b == 0) {
			r++;
			continue;
		}

		c = b & 31;

		if((b & 32) != 0) {
			depackedS3Mpat[c][r][0] = *buffer++;
			depackedS3Mpat[c][r][1] = *buffer++;
		}

		if((b & 64) != 0) {
			depackedS3Mpat[c][r][2] = *buffer++;
		}

		if((b & 128) != 0) {
			depackedS3Mpat[c][r][3] = *buffer++;
			depackedS3Mpat[c][r][4] = *buffer++;
		}
	}

	return;
}

void convertpattern(FILE* STM) {
	unsigned int r = 0, c = 0;
	unsigned char note = 0xFF, ins = 0, vol = 0xFF, eff = 0x00, prm = 0x00;

	for(c = 0; c < 4; c++) {
		for(r = 0; r < 64; r++) {
			note = depackedS3Mpat[c][r][0];
			ins = depackedS3Mpat[c][r][1], vol = depackedS3Mpat[c][r][2];
			eff = depackedS3Mpat[c][r][3], prm = depackedS3Mpat[c][r][4];

			converteffect(eff, prm);
		}
	}

	return;
}

void converteffect(unsigned char eff, unsigned char prm) {
	switch(eff) {
		/* no effect */
		case 0:
		break;

		/* set tempo */
		case 1:
		prm <<= 4;
		break;

		/* set next order */
		case 2:
		puts("WARNING:\tThis does not automatically perform a pattern break!\a");
		break;

		/* pattern break */
		case 3:
		if (prm != 0) puts("WARNING:\tRow cannot be specified with a pattern break!\a");
		break;

		/* volume slide */
		case 4:
		if (prm == 0) puts("WARNING:\tThere's no effect memory!\a");
		if (prm & 0xF0 || prm & 0x0F) puts("WARNING:\tNo fine volume slides!\a");
		break;

		/* up/down portamento slide*/
		case 5:
		case 6:
		if (prm == 0) puts("WARNING:\tThere's no effect memory!\a");
		if (prm >= 0xE0 || prm >= 0xF0) puts("WARNING:\tNo fine/extra-fine portamento slides!\a");
		break;

		/* tone portamento */
		case 8:
		goto noeffectmem;
		break;

		/* arpeggio */
		case 0xA:
		puts("WARNING:\tArpeggio is pretty broken!\a");
		break;

		default:
		printf("WARNING:\tUnknown effect %c%02X!\a", (eff + 0x30) - 1, prm);
		break;
	}

	return;
}

/* MISC. */
void showsampleheader(unsigned char* buffer) {
	printf("name:\t\t%s\n"
		   "filename:\t%.12s\n"
		   "length:\t\t%06u\n"
		   "loopstart:\t%06u\n"
		   "loopend:\t%06u\n"
		   "cfreq:\t\t%05u\n"
		   "flags:\t\t%01X\n"
		   "type:\t\t%01d\n",
		   (char *)&buffer[48],
		   (char *)&buffer[1],
		   (buffer[17] << 8) + buffer[16],
		   (buffer[19] << 8) + buffer[18],
		   (buffer[21] << 8) + buffer[20],
		   (buffer[33] << 8) + buffer[23],
		   buffer[31],
		   buffer[0]);
	return;
}

void showsongheader(unsigned char* buffer) {
	printf("Module name:\t%s\n"
	"Order count:\t%d\n"
	"Instruments:\t%d\n"
	"Patterns:\t%d\n"
	"TPR:\t\t%02X\n"
	"TPS:\t\t%02X\n",
	(char *)&buffer[0],
	orderCnt,
	instCnt,
	patCnt,
	ticksPerRow,
	ticksPerSec);
	return;
}
unsigned int convertfromparapointer(const unsigned int parapointer) {
	return parapointer << 4;
}

unsigned int convert2parapointer(const unsigned int pointer) {
	return pointer >> 4;
}

unsigned int calculatepadding(const unsigned int position) {
	return (16 - (position % 16)) % 16;
}

/* END OF FILE */
