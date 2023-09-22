#include <stdio.h>
#include <stdlib.h>
#include "pattern.h"

unsigned char* s3mPat;
unsigned short patptrArray[255];

unsigned char channelRemap[32] = {
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255,
};

/* 64 rows, 16 channels, 5 bytes */
unsigned char s3mPatArray[64][16][5];

int readS3MPattern(FILE *inS3M, unsigned short pointer) {
	register unsigned char cv = 0;
	register unsigned char r = 0, c = 0;
	unsigned char *p;
	unsigned char s3mNote = 255, s3mIns = 0, s3mVol = 255, s3mEff = 0, s3mParam = 0;
	unsigned short packedlen;
	fseek(inS3M, pointer, SEEK_SET);

	fread(&packedlen, sizeof(char), 2, inS3M);

	/* "packed_len includes its own length, so as it is two bytes long, the length of the packed data will be two bytes less than this value."
	- https://moddingwiki.shikadi.net/wiki/S3M_Format#Patterns */

	packedlen -= 2;

	s3mPat = (unsigned char*)calloc(packedlen, sizeof(char));
	if (s3mPat == NULL) {
		puts("Failed to allocate memory.");
		return 2;
	}

	fread(s3mPat, sizeof(char), packedlen, inS3M);

	p = s3mPat;

	/* for every row... */
	for (; r < 64; ++r) {

		/* do this in an infinite loop */
		while(1) {
			/* read a byte */
			cv = *(p++);

			/* if it is 0 then the row is over */
			if (cv == 0) break;

			c = channelRemap[cv & 31];

			/* if there's a note/instrument present */
			if (cv & 0x20) {
				s3mNote = *(p++);
				s3mIns = *(p++);
			} else {
				s3mNote = 255;
				s3mIns = 0;
			}

			/* if there's a volume field present */
			if (cv & 0x40) {
				s3mVol = *(p++);
			} else {
				s3mVol = 255;
			}

			/* if there's an effect present */
			if (cv & 0x80) {
				s3mEff = *(p++);
				s3mParam = *(p++);
			} else {
				s3mEff = 0;
				s3mParam = 0;
			}
		}

		printf("row %02u channel %02u: %02X %02X %02X %c%02X\n", r, c, s3mNote, s3mIns, s3mVol, ('A' - 1) + s3mEff, s3mParam);

		s3mPatArray[r][c][0] = s3mNote, s3mPatArray[r][c][1] = s3mIns, s3mPatArray[r][c][2] = s3mVol, s3mPatArray[r][c][3] = s3mEff, s3mPatArray[r][c][4] = s3mParam;
	}

	free(s3mPat);

	return 0;
}

int convertPattern(FILE* outSTM) {
	unsigned char stPat[4];
	register unsigned char r = 0, c = 0;
	unsigned char s3mNote, s3mIns, s3mVol, s3mEff, s3mParam;

	/* for 4 channels... */
	for (c = 0; c < 4; ++c) {

		/* for every row... */
		for (r = 0; r < 64; ++r) {

			/* assign each variable to the specific row and channel */
			s3mNote = s3mPatArray[r][c][0], s3mIns = s3mPatArray[r][c][1], s3mVol = s3mPatArray[r][c][2], s3mEff = s3mPatArray[r][c][3], s3mParam = s3mPatArray[r][c][4];

			switch (s3mEff) {
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
				/* quirks are handled here */
				switch (s3mEff) {

				/* A - has a scaling factor */
				case 1:
					s3mParam <<= 4;
					break;
				/* B - does not break immediately*/
				case 2:
					printf("WARNING: row %02u channel %02u - effect B does not break immediately, pair it with a C/Pattern Break if you expect this behavior!\n", r, c);
					break;
				/* D - no fine volume slides */
				case 4:
					if ((s3mParam & 0x0F) == 0x0F || (s3mParam & 0xF0) == 0xF0)
						printf("WARNING: row %02u channel %02u - Fine volume slides are not supported!\n", r, c);
					break;
				/* E/F - no fine/extra fine portamento slides */
				case 5:
				case 6:
					if (s3mParam >= 0xE0 || s3mParam >= 0xF0)
						printf("WARNING: row %02u channel %02u - Fine/Extrafine portamento slides are not supported!\n", r, c);
					break;
				}
				break;

			default:
				printf("WARNING: row %02u channel %02u - Effect %c is not supported!\n", r, c, ('A' - 1) + s3mEff);
				s3mEff = 0;
				break;
			}

			/* Annoyingly, Scream Tracker 2's TECH.DOC only mentions *decoding* this, not *encoding*..
			* (shown below with cleaned up formatting)
			* note = [BYTE0] & 15 (C=0,C#=1,D=2...)
			* octave = [BYTE0] / 16 (or shift right 4)
			* instrument = [BYTE1] / 8 (or shift right 3)
			* volume = ([BYTE1] & 7) + [BYTE2] / 2 (or shift right 1)
			* command = [BYTE2] & 15
			* command info = [BYTE3]
			*/

			stPat[0] = s3mNote;
			stPat[1] = (s3mIns & 31) >> 3; /* 0b00011111 */
			stPat[1] |= s3mVol & 7;
			stPat[2] = (s3mVol >> 3) & 5;
			stPat[2] |= s3mEff & 15;
			stPat[3] = s3mParam;

			/* printf("row %02u channel %02u: %02X %02X %02X %02X\n", r, c, stPat[0], stPat[1], stPat[2], stPat[3]); */

			fwrite(stPat, sizeof(char), 4, outSTM);
		}
	}

	return 0;
}
