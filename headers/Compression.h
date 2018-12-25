
#ifndef COMPRESSOR_COMPRESSION_H
#define COMPRESSOR_COMPRESSION_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#define MAX_CODE 256

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
    unsigned long long left;
    unsigned long long mid_left;
    unsigned long long mid_right;
    unsigned long long right;
} Code256Bits;

/********************** Compression's Functions Declaration ******************************/

void initializeTable(Element *ptrElements);

void calculateFrequencies(Element *ptrElements, unsigned char *buffer, int bufferSize);

int compareByWord(const void *a, const void *b);

void orderByFreqDesc(Element *ptrElements);

void orderBycodeLengthCresc(Element *ptrElements);

long sumFrequencies(Element *ptrElements, Node *root);

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

void printCodes(Element *ptrElements);

void compress(char *toCompFileName, char *compFileName);


#endif //COMPRESSOR_COMPRESSION_H
