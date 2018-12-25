
#ifndef COMPRESSOR_DECOMPRESSIONFUNCTIONS_H
#define COMPRESSOR_DECOMPRESSIONFUNCTIONS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "Compression.h"


/********************** Decompression's Functions Declaration ******************************/

int getFileSize(FILE *inputFile);

void readHeader(FILE *inputfile, Element *ptrElements);

void readCompressedFile(FILE *inputFile, unsigned int bufferSize, unsigned char *buffer);

void createDecodingTree(Node *root, const char *code, int index, unsigned char word);

void decode(Node *root, Element *ptrElements);

int extractBitOnPosition(unsigned char byte, int bitPosition);

char *byteToChars(unsigned char byte);

void writeDecompressedFile(FILE *outputFile, Node *root, unsigned char *buffer, int bufferSize);

void decompress(char *compFileName, char *decompFileName);


#endif //COMPRESSOR_DECOMPRESSIONFUNCTIONS_H
