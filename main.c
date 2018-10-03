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


typedef struct frequenciesTable {
    unsigned char word;
    int frequencies;
} FrequenciesTable;

typedef struct treeNode {
    int start, end;
    struct TreeNode *right;
    struct TreeNode *left;
} TreeNode;


void printMyArray(FrequenciesTable *pTable);


void initilizeTable(FILE *pIobuf, FrequenciesTable *pTable);

void calculateFrequencies(FILE *file, FrequenciesTable *table);


int main() {
    // memory allocation for an array of 256 elements (the frequencies' table)
    FrequenciesTable *table = (FrequenciesTable *)malloc(ASCII_CHARACHTER * sizeof(FrequenciesTable));
    FILE *file = fopen("prova.txt","rb");
    initilizeTable(file, table);
    calculateFrequencies(file,table);
    printMyArray(table);
    return 0;
}

void initilizeTable(FILE *pIobuf, FrequenciesTable *pTable) {
    for (int i = 0; i < ASCII_CHARACHTER; ++i) {
        pTable[i].word=i;
        pTable[i].frequencies=0;
    }
}



void printMyArray(FrequenciesTable *pTable) {
    for (int i = 0; i < ASCII_CHARACHTER; ++i) {
        printf("carattere: %d  %d\n", pTable[i].word,pTable[i].frequencies);
    }
}

void calculateFrequencies(FILE *file, FrequenciesTable *table) {
    int ch ;
    while ((ch = fgetc(file)) != EOF){
        table[ch].frequencies++;
    }
}
