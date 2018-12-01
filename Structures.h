
#ifndef COMPRESSOR_STRUCTURES_H
#define COMPRESSOR_STRUCTURES_H


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
    char code[256];
} Code;


#endif //COMPRESSOR_STRUCTURES_H
