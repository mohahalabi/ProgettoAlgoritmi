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

#define ASCII_CHARACHTER 256

typedef struct element {
    unsigned char word;
    int frequencies;
} Element;

typedef struct treeNode {
    int start, end;
    struct TreeNode *right;
    struct TreeNode *left;
} TreeNode;

void printMyArray(Element *pElement);

void initializeTable(Element *pElement);

void calculateFrequencies(FILE *file, Element *pElement);

void orderDesc(Element *pElement);


int main() {
    // memory allocation for an array of 256 elements (the frequencies' table)
    Element *table = (Element *) malloc(ASCII_CHARACHTER * sizeof(Element));
    FILE *file = fopen("immagine.tiff", "rb");
    initializeTable(table);
    calculateFrequencies(file, table);
    printMyArray(table);
    orderDesc(table);
    printf("\nLa tabella ordinata:\n");
    printMyArray(table);
    return 0;
}

void initializeTable(Element *pElement) {
    for (int i = 0; i < ASCII_CHARACHTER; ++i) {
        pElement[i].word = (unsigned char) i;
        pElement[i].frequencies = 0;
    }
}

void printMyArray(Element *pElement) {
    for (int i = 0; i < ASCII_CHARACHTER; ++i) {
        printf("word:  %-6d  %-6d\n", pElement[i].word, pElement[i].frequencies);
    }
}

void calculateFrequencies(FILE *file, Element *pElement) {
    int ch;
    while ((ch = fgetc(file)) != EOF) {
        pElement[ch].frequencies++;
    }
}

int compare(const void *a, const void *b) {
    Element *element1 = (Element *) a;
    Element *element2 = (Element *) b;
    return (element2->frequencies - element1->frequencies);
}

void orderDesc(Element *pElement) {
    qsort(pElement, ASCII_CHARACHTER, sizeof(Element), compare);
}
