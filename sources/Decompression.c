
#include "../headers/Decompression.h"
#include "../headers/Compression.h"


/************************ Functions Definition *****************************/

int getFileSize(FILE *inputFile) {
    fseek(inputFile, 0, SEEK_END);
    int size = ftell(inputFile);
    return size;
}


void readLengths(FILE *inputfile, Element *ptrElements) {

    unsigned char *lengths = malloc(MAX_CODE * sizeof(unsigned char));
    fseek(inputfile, 0, SEEK_SET);
    fread(lengths, sizeof(unsigned char), MAX_CODE, inputfile);

    for (int i = 0; i < MAX_CODE; ++i) {
        ptrElements[i].word = (unsigned char) i;
        ptrElements[i].codeLength = lengths[i];
    }
    free(lengths);
}