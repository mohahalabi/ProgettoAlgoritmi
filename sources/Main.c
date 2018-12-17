/*
Shannon-Fano implementation.
****************************************************************************
Author: Mohammad Halabi
Programming language: C standard version 99
This is a prototype implementation which will be developed during the
third semester as a project of algorithms and data structures course.

****************************************************************************
*/

#include <stdio.h>
#include <string.h>
#include "../headers/Compression.h"
#include "../headers/Decompression.h"


void decompressionTests() {

    FILE *fileIn = fopen("compressed", "wb");
    unsigned char lengths[MAX_CODE];
    for (int i = 0; i < MAX_CODE; ++i) {
        lengths[i] = (unsigned char) i;
    }

    fwrite(lengths, sizeof(unsigned char), MAX_CODE, fileIn);
    fclose(fileIn);
    fopen("compressed", "rb");

    int size = getFileSize(fileIn);

    fclose(fileIn);
    printf("file's size: %d bytes\n", size);

    Element *ptrElements = (Element *) malloc(sizeof(Element) * MAX_CODE);

    fopen("compressed", "rb");
    readLengths(fileIn, ptrElements);
    fclose(fileIn);

    orderBycodeLengthCresc(ptrElements);

    for (int i = 0; i < MAX_CODE; ++i) {
        printf("word: %d   : %d\n", ptrElements[i].word, ptrElements[i].codeLength);
    }

}


int main() {


    compress();
    //decompressionTests();

    //printf("%s", fromNumToChars(17, 5));

    return 0;
}
