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
#include "c_cmn.h"

/* thanks A.I.! */
cc_u32l crc32(cc_u8l buf[], cc_u32l buf_len)
{
	cc_u32l hash = 0, mask = 0;
	register cc_u32l i = 0, j = 0;
	for (; i < buf_len; ++i) {
		hash ^= buf[i];

		for (j = 0; j < 8; ++j) {
			mask = -(hash & 1);
			hash = (hash >> 1) ^ (0xEDB88320 & mask);
		}
	}

	return ~hash;
}

cc_u32l xorshift32(register cc_u32l state)
{
	state ^= state << 13;
	state ^= state >> 17;
	state ^= state << 5;
	return state;
}


int main(int argc, char *argv[]) {

	/* I'm only able to write in bytes at a time,
	and I have to dynamically allocate memory! */
	char *s3mHeader;
	cc_u8l s3minstheader[80];
	char *s3mPat;
	char *orderArray;
	cc_u8l *sampleData;

	/* I can't use dynamic allocation easily (mainly due to it defaulting to being signed...damn x86..) */
	cc_u16l patptrArray[255];
	cc_u16l instptrArray[99];
	cc_u16l instdatptrArray[99];

	/* pattern, row, channel, sample, order, length, name */
	register cc_u8l p = 0, r = 0, c = 0, s = 0, o = 0, l = 0, n = 0;

	/* for sample conversion */
	cc_u32l parapointer = 0;
	cc_u32l crc = 0;
	cc_u32l rng = 0;

	/* counters */
	register cc_u8l ordCnt = 0;
	register cc_u8l patCnt = 0;
	register cc_u8l insCnt = 0;

	/* for pattern conversion */
	cc_u8l cv = 0;
	cc_u16l patSize = 0;
	cc_u32l stPat = 0xFF018000;
	register cc_u8l s3mNote = 255, s3mIns = 0, s3mVol = 255, s3mEff = 0, s3mParam = 0;

	puts("Screamverter by RepellantMold (2023)");

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

		/* Copy over the sample data */
		for (s = 0; s < 31; ++s) {
			/* clear the header */
			memset(stmSampHeader, 0, sizeof(stmSampHeader));
			if (s < insCnt) {
				fseek(inS3M, instptrArray[s], SEEK_SET);
				/* printf("%X - %lX\n", instptrArray[s], ftell(inS3M)); */
				fread(s3minstheader, sizeof(char), 80, inS3M);

				/* if the sample itself is not a message */
				if (s3minstheader[0]) {
					/* this will be saved for later for when sample data itself has to be converted */
					parapointer = (s3minstheader[13] << 16) + (s3minstheader[15] << 8) + s3minstheader[14];

					/* turn the parapointers into regular pointers */
					instdatptrArray[s] = parapointer << 4;

					/* if the file name is not blank in the instrument */
					if (s3minstheader[1]) {
						/* file name */
						memcpy(stmSampHeader, &s3minstheader[1], 12);
					} else if (s3minstheader[48]) {
						/* copy the sample name */
						memcpy(stmSampHeader, &s3minstheader[48], 8);

						/* perform a CRC32 on the sample name */
						crc = crc32(&s3minstheader[1], sizeof(char) * 12);
						stmSampHeader[9] = '.';
						stmSampHeader[10] = ((crc & 9) >> 16) + 0x30;
						stmSampHeader[11] = ((crc & 9) >> 8) + 0x30;
						stmSampHeader[12] = (crc & 9) + 0x30;
					} else {
						/* perform a CRC32 on the entire sample header then put it into ASCII */
						crc = crc32(s3minstheader, sizeof(s3minstheader));
						rng = xorshift32(1337);

						for (l = 0; l < 9; ++l)
							/* use a random number and put the value into ASCII */
							stmSampHeader[l] = CC_CLAMP('0', '9', rng & 0x0F);
						
						stmSampHeader[9] = '.';
						stmSampHeader[10] = ((crc & 9) >> 16) + 0x30;
						stmSampHeader[11] = ((crc & 9) >> 8) + 0x30;
						stmSampHeader[12] = (crc & 9) + 0x30;
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

					stmSampHeader[16] = s3minstheader[16];
					stmSampHeader[17] = s3minstheader[17];

					/* default volume */
					stmSampHeader[22] = s3minstheader[28];

					/* c2 speed */
					if (s3minstheader[34] || s3minstheader[35])
						puts("WARNING: C2 speed is too high, it will be truncated!");

					stmSampHeader[24] = s3minstheader[32];
					stmSampHeader[25] = s3minstheader[33];

					stmSampHeader[22] = s3minstheader[28];
				} else {
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

					/* c2 speed */
					stmSampHeader[24] = 81;
					stmSampHeader[25] = 92;
				}

			} else {
				/* default stuff if there's no other samples */

				for (n = 0; n < 12; ++n) {
					stmSampHeader[n] = 0;
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
			if ((cc_u8l)orderArray[o] < 254) {
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

			fread(s3mPat, sizeof(char), sizeof(s3mPat), inS3M);

			/* TODO: get something functional...
			while(1) {
				cv = *(s3mPat++);
				if (0 == cv) break;

				if((cv & 0x20) != 0) {
					s3mNote = *(s3mPat++);
					s3mIns = *(s3mPat++);
				}
				
				if((cv & 0x40) != 0) {
					s3mVol = *(s3mPat++);
				}

				if((cv & 0x80) != 0) {
					s3mEff = *(s3mPat++) & 0x1F;
					s3mParam = *(s3mPat++);
				}

				printf("Channel %02u: Note: %02X, Octave: %02X, Instrument: %02X, Volume: %02X, Effect: %02X, Parameter: %02X\n", cv & 31, s3mNote, s3mNote & 0x0F, s3mIns, s3mVol, s3mEff, s3mParam);

			}
			*/

			free(s3mPat);
		}

		/* now grab the data */
		for (s = 0; s < 31; ++s) {
			/* clear the header */
			fseek(inS3M, instdatptrArray[s], SEEK_SET);
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
