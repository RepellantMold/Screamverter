#include <stdio.h>
#include <stdlib.h>
#include "pattern.h"

char* s3mPat;
unsigned short patptrArray[255];

unsigned char s3mPatArray[5*64];

int convertPattern(FILE *inS3M, FILE* outSTM, unsigned short pointer) {
	register unsigned char cv = 0;
	unsigned char stPat[4];
	unsigned char s3mNote = 255, s3mIns = 0, s3mVol = 255, s3mEff = 0, s3mParam = 0;
	register unsigned char r = 0, c = 0;
	unsigned short packedlen;

	fseek(inS3M, pointer, SEEK_SET);

	fread(&packedlen, sizeof(char), 2, inS3M);

	/* "packed_len includes its own length, so as it is two bytes long, the length of the packed data will be two bytes less than this value."
	- https://moddingwiki.shikadi.net/wiki/S3M_Format#Patterns */
	packedlen -= 2;

	s3mPat = (char*)calloc(packedlen, sizeof(char));
	if (s3mPat == NULL) {
		puts("Failed to allocate memory.");
		return 2;
	}

	fread(s3mPat, sizeof(char), packedlen, inS3M);

	char *p = s3mPat;

	/* for every row... */
	for (; r < 64; ++r) {
		/* repeat this for each channel */
		while (1) {
			/* reset all the variables */
			s3mNote = 255, s3mIns = 0, s3mVol = 255, s3mEff = 0, s3mParam = 0;
			stPat[0] = 0xFF;
			stPat[1] = 0x01;
			stPat[2] = 0x80;
			stPat[3] = 0x00;

			cv = *(p++);
			/* if cv is 0 then the row is over */
			if (cv == 0) break;

			c = cv & 15;

			/* if there's a note/instrument present */
			if (cv & 0x20) {
				s3mNote = *(p++);
				s3mIns = *(p++);
			}

			if (cv & 0x40) {
				s3mVol = *(p++);
			}

			if (cv & 0x80) {
				s3mEff = *(p++) & 0x1F;
				s3mParam = *(p++);
			}

			/* Annoyingly, Scream Tracker 2's TECH.DOC only mentions *decoding* this, not *encoding*..
			 * (shown below with cleaned up formatting)
			 * note = [BYTE0] & 15 (C=0,C#=1,D=2...)
			 * octave = [BYTE0] / 16 (or shift right 4)
			 * instrument = [BYTE1] / 8 (or shift right 3)
			 * volume = ([BYTE1] & 7) + [BYTE2] / 2 (or shift right 1)
			 * command = [BYTE2] & 15
			 * command info = [BYTE3]
			 * Why Sami made the pattern format in Scream Tracker 2 this way, I have no clue... */
			stPat[0] = s3mNote;
			stPat[1] |= (s3mIns >> 3) & 31; /* 0b00011111 */
			stPat[1] |= s3mVol & 0x07;
			stPat[2] |= (s3mVol >> 3) & (0xFF << (8 - 3));
			stPat[2] |= s3mEff & 0x07;
			stPat[3] = s3mParam;

			printf("%02u: %02X, %02X, %02X, %02X%02X = %08X\n", c, s3mNote, s3mIns, s3mVol, s3mEff, s3mParam, (stPat[0] + (stPat[1] << 8) + (stPat[2] << 16) + (stPat[3] << 24)));

			fwrite(stPat, sizeof(char), 4, outSTM);
		}
	}
	
	free(s3mPat);

	return 0;
}