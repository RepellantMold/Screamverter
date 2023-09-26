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

int main(int argc, char *argv[])
{
    puts("Screamverter, a Scream Tracker 3 module to 2 downgrader"
         "by RepellantMold (2023)\n"
         "This code is licensed under MIT-0.\n");
    if (argc == 3) {

    } else if (argc > 3) {
        puts("Too many arguments");
        return 1;
    } else {
        puts("Expected usage: screamverter input.s3m output.stm");
    }
    return 0;
}
