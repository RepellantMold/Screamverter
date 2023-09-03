/*
 * Screamverter by RepellantMold (2023)
 * This code is licensed under MIT-0.

 * I made sure to test this with a multitude of compilers I have available.

 * Compiled on:
 * Bloodshed's Dev-C++ 5.11 (in a Windows XP virtual machine),
 * Microsoft Quick C (MS-DOS via DOSBox-X),
 * Tiny C Compiler (Windows 11 64-bit),
 * TDM-GCC (Windows 11 64-bit)

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
	char *s3mheader;
	char *s3minstheader;
	char *stmheader;
	char *s3mpat;
	char *stmpat;
	char *orderarray;
	char *patptrarray;
	char *instptrarray;

	register unsigned char p, r, c, s, o, l = 0;

	unsigned char orderlen;
	unsigned char numofpats;
	unsigned char numofinsts;

	unsigned short trackerver;
	
	puts("Screamverter\nby RepellantMold (2023)");

	if( argc == 3 ) {
		FILE *ins3m;
		FILE *outstm;
		ins3m = fopen(argv[1], "rb");
		if (ins3m == NULL) {
			puts("Failed to open the file.");
			return 1;
		}

		outstm = fopen(argv[2], "wb");
		if (outstm == NULL) {
			puts("Failed to write the file.");
			return 1;
		}

		s3mheader = (char*)calloc(96, sizeof(char));
		if (s3mheader == NULL) {
			puts("Failed to allocate memory!");
			return 2;
		}

		fread(s3mheader, 96, 1, ins3m);
		
		puts("Converting header...");

		orderlen = s3mheader[32];
		numofpats = s3mheader[36];

		stmheader = (char*)calloc(1040, sizeof(char));
		if (stmheader == NULL) {
			puts("Failed to allocate memory!");
			return 2;
		}

		/* Copy over the title string */
		memcpy(stmheader, s3mheader, 20);

		/* add a terminator if needed */
		if (stmheader[19] != 0) {
			stmheader[19] = 0;
		}

		/* "!Scream!" */
		stmheader[20] = 0x21;
		stmheader[21] = 0x53;
		stmheader[22] = 0x63;
		stmheader[23] = 0x72;
		stmheader[24] = 0x65;
		stmheader[25] = 0x61;
		stmheader[26] = 0x6D;
		stmheader[27] = 0x21;

		/* DOS EoF */
		stmheader[28] = 0x1A;

		/* Module */
		stmheader[29] = 0x02;

		/* Scream Tracker 2.21 */
		stmheader[30] = 0x02;
		stmheader[31] = 0x15;

		/* Initital tempo (not dealing with the weird scaling factor) */
		stmheader[32] = s3mheader[49] << 4;

		/* Number of patterns */
		stmheader[33] = s3mheader[36];

		/* Global volume */
		stmheader[34] = s3mheader[48];
		
		/* printf("%X", ftell(ins3m)); */

		orderarray = (char*)calloc(255, sizeof(char));
		if (orderarray == NULL) {
			puts("Failed to allocate memory for the STM header.");
			return 2;
		}
		
		fread(orderarray, sizeof(char), orderlen, ins3m);

		for (; o < orderlen; ++o) {
			if ((unsigned char)orderarray[o] < 254) {
				orderarray[l] = orderarray[o];
				++l;
				if ((unsigned char)orderarray[o] > numofpats)
					numofpats = (unsigned char)orderarray[o];
			}
		}

		printf("Orders (excluding pattern markers) found: %u\n", l);

		fwrite(stmheader, 1040, sizeof(char), outstm);

		free(stmheader);
		free(s3mheader);
		free(orderarray);

		fclose(ins3m);
		fclose(outstm);
		return 0;
	} else if( argc > 3 ) {
		puts("Too many arguments.");
		return 1;
	} else {
		puts("Expected usage: screamverter <filename.s3m> <filename.stm>");
		return 1;
	}
}
