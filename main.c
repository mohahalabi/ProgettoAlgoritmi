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

#define ASCII_CODE 256

typedef struct {
    unsigned char word;
    int frequencies;
} Element;

typedef struct {
    int start, end;
    struct Node *rightChild;
    struct Node *leftChild;
} Node;

void printMyArray(Element *pElement);

void initializeTable(Element *pElement);

void calculateFrequencies(FILE *file, Element *pElement);

void orderDesc(Element *pElement);

int getSplitIndex(Element *pElement);

Node *createNode(Node *lNode, Node *rNode, int start, int end);

void createTheTree(Element *pElement, Node *root);


void initializeTable(Element *pElement) {
    for (int i = 0; i < ASCII_CODE; ++i) {
        pElement[i].word = (unsigned char) i;
        pElement[i].frequencies = 0;
    }
}

void printMyArray(Element *pElement) {
    for (int i = 0; i < ASCII_CODE; ++i) {
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
    qsort(pElement, ASCII_CODE, sizeof(Element), compare);
}

int getSplitIndex(Element *pElement) {
    long sumOfFrequencies = 0;
    long splitIndex, halfOfSum;
    long sum = 0;
    for (int i = 0; i < ASCII_CODE; ++i) {
        sumOfFrequencies = sumOfFrequencies + pElement[i].frequencies;
    }
    halfOfSum = sumOfFrequencies / 2;
    for (int j = 0; j < ASCII_CODE; ++j) {
        sum = sum + pElement[j].frequencies;
        if (sum >= halfOfSum) {
            splitIndex = j + 1;
            printf("somma delle frequenze = %ld\n", sumOfFrequencies);
            printf("dove dividere:  %ld\n", splitIndex);
            return splitIndex;
        }
    }

}

Node *createNode(Node *lNode, Node *rNode, int start, int end) {
    Node *node = (Node *) malloc(sizeof(Node));
    node->leftChild = lNode;
    node->rightChild = rNode;
    node->start = start;
    node->end = end;
    return node;
}

void createTheTree(Element *pElement, Node *root) {
    int splitIndex = getSplitIndex(pElement);

    if (root->start == root->end) {
        return;
    }

    if (root->start == (root->end - 1)) {
        Node *leftNode = createNode(NULL, NULL, root->start, root->start);
        Node *rightNode = createNode(NULL, NULL, root->end, root->end);
        root->leftChild = leftNode;
        root->rightChild = rightNode;
        return;
    } else {
        Node *leftNode = createNode(NULL, NULL, root->start, splitIndex);
        Node *rightNode = createNode(NULL, NULL, splitIndex + 1, root->end);
        root->leftChild = leftNode;
        root->rightChild = rightNode;
        createTheTree(pElement, root->leftChild);
        createTheTree(pElement, root->rightChild);
    }


}

int main() {

    Element *table = (Element *) malloc(ASCII_CODE * sizeof(Element));
    FILE *file = fopen("daComprimere", "rb");
    initializeTable(table);
    calculateFrequencies(file, table);
    printMyArray(table);
    orderDesc(table);
    printf("\nLa tabella ordinata:\n");
    printMyArray(table);
    getSplitIndex(table);
    //Node *root = createNode(NULL,NULL,0,ASCII_CODE-1);
    //createTheTree(table,root);
    return 0;
}