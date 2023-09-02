/*
 * Screamverter by RepellantMold (2023)
 * This code is licensed under MIT-0.
 * 
 * Tested on:
 * Bloodshed's Dev-C++ 5.11 (in a Windows XP virtual machine),
 * Borland Turbo C++ (MS-DOS via DOSBox-X),
 * Tiny C Compiler (Windows 11 64-bit),
 * TDM-GCC (Windows 11 64-bit)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

	/* This creates a whole buttload of code that you'll have to see when scrolling down
	 * suppose it'd be at least somewhat portable? */
	char s3mheader[96];
	char s3minstheader[80];
	char stmheader[1040];
	
	unsigned char p, r, c, s, o, l = 0;

	unsigned char orderlen;
	unsigned char numofpats;
	unsigned char numofinsts;

	unsigned short trackerver;
	
	unsigned char orderarray[255];
	
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

		fread(s3mheader, 96, 1, ins3m);
		
		puts("Converting header...");

		orderlen = s3mheader[32];
		numofpats = s3mheader[36];

		/* Convert the song title to the STM header (though this could be problematic if the title is too long) */
		memcpy(stmheader, s3mheader, 20);

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
		
		fread(orderarray, sizeof(char), orderlen, ins3m);

		for (; o < orderlen; ++o) {
			if (orderarray[o] < 254) {
				orderarray[l] = orderarray[o];
				l++;
				if (orderarray[o] > numofpats)
					numofpats = orderarray[o];
			}
		}

		fwrite(stmheader, 1040, 1, outstm);

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
