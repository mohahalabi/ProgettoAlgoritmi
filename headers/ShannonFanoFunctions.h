
#ifndef COMPRESSOR_COMPRESSION_H
#define COMPRESSOR_COMPRESSION_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <time.h>

#define MAX_CODE 256


/*************************** Structures ***********************************/

typedef struct element {
    unsigned char word;
    int frequency;
    char code[256];
    unsigned char codeLength;
} Element;

typedef struct node {
    int start, end;
    struct node *rightChild;
    struct node *leftChild;
} Node;

typedef struct code {
    char code[256];
} Code;

typedef struct codeBits {
//    unsigned long long left;
//    unsigned long long mid_left;
//    unsigned long long mid_right;
    unsigned long long right;
} CodeBits;


/********************** Compression's Functions Declaration ******************************/

void initializeElements(Element *ptrElements);

void fileIntoBuffer(FILE *inputFile, unsigned int fileSize, unsigned char *buffer);

void calculateFrequencies(Element *ptrElements, unsigned char *buffer, int bufferSize);

void orderByFreqDesc(Element *ptrElements);

void orderBycodeLengthCresc(Element *ptrElements);

void orderByWord(Element *ptrElements);

long long int sumFrequencies(Element *ptrElements, Node *root);

int getSplitIndex(Element *ptrElements, Node *root);

Node *createNode(Node *lNode, Node *rNode, int start, int end);

void createEncodingTree(Element *ptrElements, Node *root);

void encode(Code *ptrCodes, Node *root);

void writeCodesForAllElements(Element *ptrElements, Code *ptrCodes);

char *fromNumToChars(long long num, int length);

void canonizeCodes(Element *ptrElements);

void writeByte(FILE *file);

void addBit(unsigned char bit, FILE *file);

void writeLengths(FILE *outputFile, Element *ptrElements);

void writeCompressedFile(unsigned char *buffer, int bufferSize, FILE *outputFile, Element *ptrElements);

//void printCodes(Element *ptrElements);  //used just for test purposes

void compress(char *toCompFileName, char *compFileName);


/********************** Decompression's Functions Declaration ******************************/

unsigned int getFileSize(FILE *inputFile);

void readHeader(FILE *inputFile, Element *ptrElements);

void readCompressedFile(FILE *inputFile, unsigned int bufferSize, unsigned char *buffer);

void createDecodingTree(Node *root, const char *code, int index, unsigned char word);

void decode(Node *root, Element *ptrElements);

int extractBitOnPosition(unsigned char byte, int bitPosition);

char *byteToChars(unsigned char byte);

void writeDecompressedFile(FILE *outputFile, Node *root, unsigned char *buffer, int bufferSize);

void decompress(char *compFileName, char *decompFileName);


#endif //COMPRESSOR_COMPRESSION_H
