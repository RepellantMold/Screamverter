#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sample.h"

unsigned short instptrArray[99];
unsigned int instdatptrArray[99];
unsigned char stmSampHeader[32];

/* thanks A.I.! */
unsigned int crc32(unsigned char buf[], unsigned int buf_len)
{
	unsigned int hash = 0, mask = 0;
	register unsigned int i = 0, j = 0;
	for (; i < buf_len; ++i) {
		hash ^= buf[i];

		for (j = 0; j < 8; ++j) {
			mask = -(hash & 1);
			hash = (hash >> 1) ^ (0xEDB88320 & mask);
		}
	}

	return ~hash;
}

unsigned int xorshift32(register unsigned int state)
{
	state ^= state << 13;
	state ^= state >> 17;
	state ^= state << 5;
	return state;
}

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

unsigned char s3minstheader[80];
unsigned short savedsamplelengths[99];
char* sampleData;

void convertSample(FILE* inS3M, unsigned short parapointer, unsigned char pptaboffs) {
	register unsigned int crc = 0, rng = 0, l = 0;

	fseek(inS3M, parapointer, SEEK_SET);
	fread(s3minstheader, sizeof(char), sizeof(s3minstheader), inS3M);

	/* if the sample itself is not a message */
	if (s3minstheader[0] == 1) {
		/* this will be saved for later for when sample data itself has to be converted */
		instdatptrArray[pptaboffs] = ((s3minstheader[13] << 16) + (s3minstheader[15] << 8) + s3minstheader[14]) << 4;

		/* if the sample name is not blank in the instrument */
		if (s3minstheader[48]) {
			/* copy the sample name (heavily truncated) */
			memcpy(stmSampHeader, &s3minstheader[48], 8);

			/* perform a CRC32 on the sample name */
			crc = crc32(&s3minstheader[48], sizeof(char) * 28);
			stmSampHeader[9] = '.';
			stmSampHeader[10] = ((crc & 9) >> 8) + '0';
			stmSampHeader[11] = ((crc & 9) >> 4) + '0';
			stmSampHeader[12] = (crc & 9) + '0';
		}
		/* otherwise if the file name isn't blank */
		else if (s3minstheader[1]) {
			/* copy the file name */
			memcpy(stmSampHeader, &s3minstheader[1], 12);
		} else {
			/* perform a CRC32 on the entire sample header then put it into ASCII */
			crc = crc32(s3minstheader, sizeof(s3minstheader));
			rng = xorshift32(1337);

			for (l = 0; l < 9; ++l)
				/* use a random number and put the value into ASCII */
				stmSampHeader[l] = '0' + (xorshift32(rng) & 9);

			stmSampHeader[9] = '.';
			stmSampHeader[10] = ((crc & 9) >> 8) + '0';
			stmSampHeader[11] = ((crc & 9) >> 4) + '0';
			stmSampHeader[12] = (crc & 9) + '0';
		}

		/* if the loop flag is set... */
		if (s3minstheader[31] & 1) {
			if (s3minstheader[22] || s3minstheader[23])
				puts("WARNING: start of loop length is too long, it will be truncated!");
			else if (s3minstheader[26] || s3minstheader[27])
				puts("WARNING: end of loop length is too long, it will be truncated!");

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
		if (s3minstheader[18] || s3minstheader[19])
			puts("WARNING: Sample length is too long, it will be truncated!");

		savedsamplelengths[pptaboffs] = (s3minstheader[17] << 8) + s3minstheader[16];
		stmSampHeader[16] = s3minstheader[16];
		stmSampHeader[17] = s3minstheader[17];

		/* default volume */
		stmSampHeader[22] = s3minstheader[28];

		/* C2 speed */
		if (s3minstheader[34] || s3minstheader[35])
			puts("WARNING: C2 speed is too high, it will be truncated!");

		stmSampHeader[24] = s3minstheader[32];
		stmSampHeader[25] = s3minstheader[33];

		stmSampHeader[22] = s3minstheader[28];
	} else {
		/* detect adlib */
		if (s3minstheader[0] >= 2)
			puts("Adlib instrument detected, THIS IS NOT SUPPORTED.");

		/* if it is a message then don't copy anything besides the title over */
		memcpy(stmSampHeader, &s3minstheader[48], 12);

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

		/* C2 speed */
		stmSampHeader[24] = 0x00;
		stmSampHeader[25] = 0x21;
	}
}

void generateBlankSample() {
	/* set a blank name */
	memset(stmSampHeader, 0, 12);

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

	/* C2 speed */
	stmSampHeader[24] = 0x00;
	stmSampHeader[25] = 0x21;
}

int convertSampleData(FILE* inS3M, FILE* outSTM, unsigned int instdatptr, unsigned short size) {
	/* if the pointer or size aren't set, ignore it */
	if (instdatptr == 0 || size == 0) return 0;
	puts("Converting sample data...");
	printf("Sample size: %u\nPointer: %08X\n", size, instdatptr);
	register unsigned int l = 0;
	unsigned char padding = 0x00;
	fseek(inS3M, instdatptr, SEEK_SET);

	sampleData = (char*)malloc(size);
	if (sampleData == NULL) {
		puts("Failed to allocate memory.");
		return 2;
	}

	fread(sampleData, sizeof(char), size, inS3M);

	for (; l < size; ++l)
		sampleData[l] = (sampleData[l] + 0x80) & 0xFF;

	fwrite(sampleData, sizeof(char), size, outSTM);

	free(sampleData);

	/* generate padding */
	fwrite(&padding, sizeof(char), 16 - (ftell(outSTM) % 16), outSTM);

	return 0;
}