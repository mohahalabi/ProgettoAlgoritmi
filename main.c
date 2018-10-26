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
    int frequencies;
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

void printMyArray(Element *ptrElement);

void initializeTable(Element *ptrElement);

void calculateFrequencies(FILE *file, Element *ptrElement);

void orderDesc(Element *ptrElement);

long sumFrequencies(Element *ptrElement, Node *root);

int getSplitIndex(Element *ptrElement, Node *root);

Node *createNode(Node *lNode, Node *rNode, int start, int end);

void createEncodingTree(Element *ptrElement, Node *root);

void encode(Code *ptrCodes, Node *root);

void writeDictionary(Element *ptrElement, Code *ptrDictionary, Code *ptrCodes);


/************************ Functions Definition *****************************/

void initializeTable(Element *ptrElement) {
    for (int i = 0; i < MAX_CODE; ++i) {
        ptrElement[i].word = (unsigned char) i;
        ptrElement[i].frequencies = 0;
    }
}

void printMyArray(Element *ptrElement) {
    for (int i = 0; i < MAX_CODE; ++i) {
        printf("word:  %-6d  %-6d\n", ptrElement[i].word, ptrElement[i].frequencies);
    }
}


void calculateFrequencies(FILE *file, Element *ptrElement) {
    int ch;
    while ((ch = fgetc(file)) != EOF) {
        ptrElement[ch].frequencies++;
    }
}


int compare(const void *a, const void *b) {
    Element *element1 = (Element *) a;
    Element *element2 = (Element *) b;
    return (element2->frequencies - element1->frequencies);
}


void orderDesc(Element *ptrElement) {
    qsort(ptrElement, MAX_CODE, sizeof(Element), compare);
}


long sumFrequencies(Element *ptrElement, Node *root) {
    long sumOfFrequencies = 0;
    for (int i = root->start; i < root->end; ++i) {
        sumOfFrequencies = sumOfFrequencies + ptrElement[i].frequencies;
    }
    return sumOfFrequencies;
}


int getSplitIndex(Element *ptrElement, Node *root) {
    long sumOfFrequencies = sumFrequencies(ptrElement, root);
    int splitIndex = 0;
    long halfOfSum;
    long sum = 0;
    halfOfSum = sumOfFrequencies / 2;
    for (int j = root->start; j < root->end; ++j) {
        sum = sum + ptrElement[j].frequencies;
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


void createEncodingTree(Element *ptrElement, Node *root) {
    int splitIndex = getSplitIndex(ptrElement, root);

    if (root->start == root->end) {
        return;
    }
    if (root->start == (root->end - 1)) {
        Node *leftNode = createNode(NULL, NULL, root->start, root->start);
        Node *rightNode = createNode(NULL, NULL, root->end, root->end);
        root->leftChild = leftNode;
        root->rightChild = rightNode;
        //return;
    } else {
        Node *leftNode = createNode(NULL, NULL, root->start, splitIndex);
        Node *rightNode = createNode(NULL, NULL, splitIndex + 1, root->end);
        root->leftChild = leftNode;
        root->rightChild = rightNode;
        createEncodingTree(ptrElement, root->leftChild);
        createEncodingTree(ptrElement, root->rightChild);
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


void writeDictionary(Element *ptrElement, Code *ptrDictionary, Code *ptrCodes) {
    for (int i = 0; i < MAX_CODE; ++i) {
        strcpy(ptrDictionary[ptrElement[i].word].code, ptrCodes[i].code);
    }

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
    printMyArray(table);
    orderDesc(table);
    printf("\nOrdered table(by frequencies):\n");
    printMyArray(table);
    Node *root = createNode(NULL, NULL, 0, MAX_CODE - 1);
    printf("\nSum of frequencies: %ld\n", sumFrequencies(table, root));
    printf("Split index: %d\n\n", getSplitIndex(table, root));

    createEncodingTree(table, root);
    encode(codes, root);
    writeDictionary(table, dictionary, codes);

    print(dictionary);

    return 0;
}