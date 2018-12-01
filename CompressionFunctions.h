
#ifndef COMPRESSOR_COMPRESSION_H
#define COMPRESSOR_COMPRESSION_H

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "Structures.h"

#define MAX_CODE 256

/********************** Functions Declaration ******************************/

void printTable(Element *ptrElements);

void initializeTable(Element *ptrElements);

void calculateFrequencies(FILE *file, Element *ptrElements);

void orderDesc(Element *ptrElements);

long sumFrequencies(Element *ptrElements, Node *root);

int getSplitIndex(Element *ptrElements, Node *root);

Node *createNode(Node *lNode, Node *rNode, int start, int end);

void createEncodingTree(Element *ptrElements, Node *root);

void encode(Code *ptrCodes, Node *root);

void writeDictionary(Element *ptrElements, Code *ptrDictionary, Code *ptrCodes);

void writeByte(FILE *file);

void addBit(unsigned char bit, FILE *file);

void writeDictionaryCanonical(Code *dict, FILE *outputFile, Element *ptrElements);

void writeCompressedFile(FILE *inputFile, FILE *outputFile, Code *dict, Element *ptrElements);

void print(Code *codes);


/************************ Functions Definition *****************************/

void initializeTable(Element *ptrElements) {
    for (int i = 0; i < MAX_CODE; ++i) {
        ptrElements[i].word = (unsigned char) i;
        ptrElements[i].frequency = 0;
    }
}

void printTable(Element *ptrElements) {
    for (int i = 0; i < MAX_CODE; ++i) {
        printf("word:  %-6d  %-6d\n", ptrElements[i].word, ptrElements[i].frequency);
    }
}


void calculateFrequencies(FILE *file, Element *ptrElements) {
    int ch;
    while ((ch = fgetc(file)) != EOF) {
        ptrElements[ch].frequency++;
    }
}


int compareStabilized(const void *a, const void *b) {
    Element *element1 = (Element *) a;
    Element *element2 = (Element *) b;
    if (element1->frequency < element2->frequency) {
        return 1;
    } else if (element1->frequency > element2->frequency) {
        return -1;
    } else {  // if zero order by word
        return element1->word - element2->word;
    }
}


void orderDesc(Element *ptrElements) {
    qsort(ptrElements, MAX_CODE, sizeof(Element), compareStabilized);
}


long sumFrequencies(Element *ptrElements, Node *root) {
    long sumOfFrequencies = 0;
    for (int i = root->start; i < root->end; ++i) {
        sumOfFrequencies = sumOfFrequencies + ptrElements[i].frequency;
    }
    return sumOfFrequencies;
}


int getSplitIndex(Element *ptrElements, Node *root) {
    long sumOfFrequencies = sumFrequencies(ptrElements, root);
    int splitIndex = 0;
    long halfOfSum;
    long sum = 0;
    halfOfSum = sumOfFrequencies / 2;
    for (int j = root->start; j < root->end; ++j) {
        sum = sum + ptrElements[j].frequency;
        if (sum >= halfOfSum) {
            splitIndex = j + 1;
            return splitIndex;
        }
    }
    return splitIndex;
}


Node *createNode(Node *lNode, Node *rNode, int start, int end) {
    Node *node = (Node *) malloc(sizeof(Node));
    node->leftChild = lNode;
    node->rightChild = rNode;
    node->start = start;
    node->end = end;
    return node;
}


void createEncodingTree(Element *ptrElements, Node *root) {
    int splitIndex = getSplitIndex(ptrElements, root);

    if (root->start == root->end) {
        return;
    }
    if (root->start == (root->end - 1)) {
        Node *leftNode = createNode(NULL, NULL, root->start, root->start);
        Node *rightNode = createNode(NULL, NULL, root->end, root->end);
        root->leftChild = leftNode;
        root->rightChild = rightNode;
    } else {
        Node *leftNode = createNode(NULL, NULL, root->start, splitIndex);
        Node *rightNode = createNode(NULL, NULL, splitIndex + 1, root->end);
        root->leftChild = leftNode;
        root->rightChild = rightNode;
        createEncodingTree(ptrElements, root->leftChild);
        createEncodingTree(ptrElements, root->rightChild);
    }

}


char charactersHolder[256] = "";

void encode(Code *ptrCodes, Node *root) {
    if (root->leftChild != NULL) {
        strcat(charactersHolder, "0");
        encode(ptrCodes, root->leftChild);
        charactersHolder[strlen(charactersHolder) - 1] = '\0';
        strcat(charactersHolder, "1");
        encode(ptrCodes, root->rightChild);
        charactersHolder[strlen(charactersHolder) - 1] = '\0';
    } else {
        strcpy(ptrCodes[root->start].code, charactersHolder);
    }
}


void writeDictionary(Element *ptrElements, Code *ptrDictionary, Code *ptrCodes) {
    for (int i = 0; i < MAX_CODE; ++i) {
        strcpy(ptrDictionary[ptrElements[i].word].code, ptrCodes[i].code);
    }
}


long bitsNumber = 0;    // All the bits written on the compressed file
unsigned char byte;     // Single byte to write on the compressed file
int currentBit = 0;     // The current bit to add, when its value is 8, I reset it to 0

void writeByte(FILE *file) {
    fputc(byte, file);
    currentBit = 0;
    byte = 0;
}


void addBit(unsigned char bit, FILE *file) {
    currentBit++;
    byte = byte << 1 | bit;
    if (currentBit == 8) {
        writeByte(file);
    }
}

void writeDictionaryCanonical(Code *dict, FILE *outputFile, Element *ptrElements) {
    for (int i = 0; i < MAX_CODE; ++i) {
        if (ptrElements[i].frequency == 0) {
            fputc(0, outputFile);
        } else {
            unsigned char length = (unsigned char) strlen(dict[i].code);
            fputc(length, outputFile);
        }
        bitsNumber += 8;
    }
}


void writeCompressedFile(FILE *inputFile, FILE *outputFile, Code *dict, Element *ptrElements) {
    int ch;
    fseek(inputFile, 0, SEEK_SET);
    writeDictionaryCanonical(dict, outputFile, ptrElements);
    while ((ch = fgetc(inputFile)) != EOF) {
        int length = strlen(dict[ch].code);
        for (int i = 0; i < length; ++i) {
            addBit((unsigned char) dict[ch].code[i], outputFile);
        }
        bitsNumber += length;
    }
    fclose(outputFile);
}


void print(Code *codes) {
    for (int i = 0; i < MAX_CODE; ++i) {
        printf("code of %d: ", i);
        for (int j = 0; codes[i].code[j] != '\0'; ++j) {
            printf("%c", codes[i].code[j]);
        }
        printf("\n");
    }
}

#endif //COMPRESSOR_COMPRESSION_H
