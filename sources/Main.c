/*
Shannon-Fano implementation.
****************************************************************************
Author: Mohammad Halabi
Programming language: C standard version 99
This is a prototype implementation which will be developed during the
third semester as a project of algorithms and data structures course.

****************************************************************************
Byte di flag: indica quanti bit sono stati aggiunti alla fine del file compresso
*/

#include <stdio.h>
#include <string.h>
#include "../headers/Compression.h"
#include "../headers/Decompression.h"


void argsErrorMessage() {
    printf("Invalid argoments!\n");
    printf("Compression's command:\n");
    printf("executableFile -c fileToCompress compressedFileName\n\n");
    printf("Decompression's command:\n");
    printf("executableFile -d compressedFileName decompressedFileName \n");
}


int isInvalidArgoments(int argc) {
    if (argc != 4) {
        argsErrorMessage();
        return 1;
    }
    return 0;
}


int isInvalidFile(char *inputFile) {
    FILE *file = fopen(inputFile, "r");
    if (!file) {
        fclose(file);
        printf("The file does not exist!\n");
        return 1;
    }
    return 0;
}


int main(int argc, char *argv[]) {

    if (isInvalidArgoments(argc) || isInvalidFile(argv[2])) {
        return 1;
    }
    if (strcmp(argv[1], "-c") == 0) {
        compress(argv[2], argv[3]);
    } else if (strcmp(argv[1], "-d") == 0) {
        decompress(argv[2], argv[3]);
    } else {
        argsErrorMessage();
        return 1;
    }
    return 0;
}
