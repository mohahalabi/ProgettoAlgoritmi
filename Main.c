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
#include "Structures.h"
#include "CompressionFunctions.h"


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

    writeCompressedFile(file, compressed, dictionary, table);

    //qsort(array, MAX_CODE, sizeof(unsigned char), compare1);
    printf("size of compressed file (in bits): %ld", bitsNumber);
    

    //TODO: Decompression

    return 0;
}
