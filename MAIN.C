/*
 * Screamverter by RepellantMold (2023)
 * This code is licensed under MIT-0.
 * It should be easily portable since this uses ANSI C.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

	char s3mheader[64];
	char s3minstheader[80];
	char stmheader[1040];

	unsigned char ordernum;

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

		fread(s3mheader, 64, 1, ins3m);

		ordernum = s3mheader[32];

		/* Convert some data to the STM header
		(I have to write bytes at a time so there's this buttload of code) */
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

		stmheader[28] = 0x1A;

		stmheader[29] = 0x02;

		stmheader[30] = 0x02;
		stmheader[31] = 0x15;

		stmheader[32] = s3mheader[49] << 4;

		stmheader[33] = s3mheader[36];

		stmheader[34] = s3mheader[48];

		fwrite(stmheader, 1040, 1, outstm);

		fclose(ins3m);
		fclose(outstm);
		return 0;
	} else if( argc > 3 ) {
		puts("Too many arguments.");
		return 1;
	} else {
		puts("Screamverter\n"
		"by RepellantMold (2023)\n\n"
		"Expected usage: screamverter <filename.s3m> <filename.stm>");
		return 1;
	}
}
