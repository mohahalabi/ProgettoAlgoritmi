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

typedef struct element {
    unsigned char word;
    int frequencies;
} Element;

typedef struct node {
    int start, end;
    struct node *rightChild;
    struct node *leftChild;
} Node;

typedef struct arrayOfString {
    char code[MAX_CODE];
} ArrayOfString;

void printMyArray(Element *pElement);

void initializeTable(Element *pElement);

void calculateFrequencies(FILE *file, Element *pElement);

void orderDesc(Element *pElement);

long sumFrequencies(Element *pElement, Node *root);

int getSplitIndex(Element *pElement, Node *root);

Node *createNode(Node *lNode, Node *rNode, int start, int end);

void createTheTree(Element *pElement, Node *root);


void initializeTable(Element *pElement) {
    for (int i = 0; i < MAX_CODE; ++i) {
        pElement[i].word = (unsigned char) i;
        pElement[i].frequencies = 0;
    }
}

void printMyArray(Element *pElement) {
    for (int i = 0; i < MAX_CODE; ++i) {
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
    qsort(pElement, MAX_CODE, sizeof(Element), compare);
}

long sumFrequencies(Element *pElement, Node *root) {
    long sumOfFrequencies = 0;
    for (int i = root->start; i < root->end; ++i) {
        sumOfFrequencies = sumOfFrequencies + pElement[i].frequencies;
    }
    return sumOfFrequencies;
}

int getSplitIndex(Element *pElement, Node *root) {
    long sumOfFrequencies = sumFrequencies(pElement, root);
    int splitIndex = 0;
    long halfOfSum;
    long sum = 0;
    halfOfSum = sumOfFrequencies / 2;
    for (int j = root->start; j < root->end; ++j) {
        sum = sum + pElement[j].frequencies;
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

void createTheTree(Element *pElement, Node *root) {
    int splitIndex = getSplitIndex(pElement, root);

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

// main procedure is currently used to test the correct
// implementation of the other procedures and functions
int main() {

    Element *table = (Element *) malloc(MAX_CODE * sizeof(Element));
    FILE *file = fopen("prova.txt", "rb");
    initializeTable(table);
    calculateFrequencies(file, table);
    printMyArray(table);
    orderDesc(table);
    printf("\nLa tabella ordinata:\n");
    printMyArray(table);
    Node *root = createNode(NULL, NULL, 0, MAX_CODE - 1);
    printf("Somma frequenze: %ld\n", sumFrequencies(table, root));
    printf("Dove splittare: %d\n", getSplitIndex(table, root));
    createTheTree(table, root);
    return 0;
}