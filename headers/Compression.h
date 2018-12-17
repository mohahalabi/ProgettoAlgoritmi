
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

} Code256;

/********************** Functions Declaration ******************************/

void printTable(Element *ptrElements);

void initializeTable(Element *ptrElements);

void calculateFrequencies(FILE *file, Element *ptrElements);

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

void writeCompressedFile(FILE *inputFile, FILE *outputFile, Code *dict, Element *ptrElements);

void compress();

void printCodes(Element *ptrElements);


#endif //COMPRESSOR_COMPRESSION_H
