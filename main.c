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
#include <stdlib.h>
#include <string.h>


#define MAX_CODE 256


/******************************* Structures ********************************/

typedef struct element {
    unsigned char word;
    int frequency;
} Element;

typedef struct node {
    int start, end;
    struct node *rightChild;
    struct node *leftChild;
} Node;

typedef struct code {
    char code[MAX_CODE];
} Code;


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

void reduceAndWriteBits(int n, FILE *file);

void writeDictionaryOnCompressedFile(Code *dict, FILE *outputFile);

void writeDictionaryCanonica(Code *dict, FILE *outputFile);

void writeCompressedFile(FILE *inputFile, FILE *outputFile, Code *dic);

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


int compare(const void *a, const void *b) {
    Element *element1 = (Element *) a;
    Element *element2 = (Element *) b;
    return (element2->frequency - element1->frequency);
}


void orderDesc(Element *ptrElements) {
    qsort(ptrElements, MAX_CODE, sizeof(Element), compare);
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


void reduceAndWriteBits(int n, FILE *file) {

    // Size of an integer is assumed to be 32 bits
    // but my number can't be more than 255 =2^8 -1, so I extract
    // just the 8 LSB from 32 bits
    for (int i = 7; i >= 0; --i) {
        int k = n >> i;
        if (k & 1)
            addBit(1, file);
        else
            addBit(0, file);
        bitsNumber++;
    }
}

// I write the dictionary like that: 8 bits to indicate how many bits the word
// is long, and then directly I write the word.
// for example: the word 1010 (its length is 4 bits) is coded: 00000100 1010
void writeDictionaryOnCompressedFile(Code *dict, FILE *outputFile) {

    for (int i = 0; i < MAX_CODE; ++i) {
        int length = strlen(dict[i].code);
        reduceAndWriteBits(length, outputFile);
        for (int j = 0; j < length; ++j) {
            addBit((unsigned char) dict[i].code[j], outputFile);
            bitsNumber++;
        }
    }
}

unsigned char array[MAX_CODE];

void writeDictionaryCanonica(Code *dict, FILE *outputFile) {
    for (int i = 0; i < MAX_CODE; ++i) {
        int length = strlen(dict[i].code);
        reduceAndWriteBits(length, outputFile);
        array[i] = (unsigned char) length;
        bitsNumber += 8;
    }
}


void writeCompressedFile(FILE *inputFile, FILE *outputFile, Code *dic) {
    int ch;
    //writeDictionaryOnCompressedFile(dic, outputFile);
    fseek(inputFile, 0, SEEK_SET);
    writeDictionaryCanonica(dic, outputFile);
    while ((ch = fgetc(inputFile)) != EOF) {
        int length = strlen(dic[ch].code);
        for (int i = 0; i < length; ++i) {
            addBit((unsigned char) dic[ch].code[i], outputFile);
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

// main procedure is currently used to test the correct
// implementation of the other procedures and functions
int main() {

    Element *table = (Element *) malloc(MAX_CODE * sizeof(Element));
    Code *codes = (Code *) malloc(MAX_CODE * sizeof(Code));
    Code *dictionary = (Code *) malloc(MAX_CODE * sizeof(Code));
    FILE *file = fopen("alice.txt", "rb");
    initializeTable(table);
    calculateFrequencies(file, table);
    printTable(table);
    orderDesc(table);
    printf("\nOrdered table(by frequencies):\n");
    printTable(table);
    Node *root = createNode(NULL, NULL, 0, MAX_CODE - 1);
    printf("\nSum of frequencies: %ld\n", sumFrequencies(table, root));
    printf("Split index: %d\n\n", getSplitIndex(table, root));

    createEncodingTree(table, root);
    encode(codes, root);
    writeDictionary(table, dictionary, codes);
    print(dictionary);

    FILE *compressed = fopen("compressed", "wb");

    writeCompressedFile(file, compressed, dictionary);

    printf("size of compressed file (in bits): %ld", bitsNumber);
    for (int i = 0; i < MAX_CODE; ++i) {
        printf("%d : %d\n", i, array[i]);
    }
    // Compression is almost done, still need some tests.
    // Consider using fread() and fwrite() instead of fgetc() and fputc()
    // Consider sorting with a stable algorithm
    // Find alternative way to communicate the dictionary

    //TODO: Decompression

    return 0;
}
