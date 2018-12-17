#include "../headers/Compression.h"


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


int compareByFreqStabilized(const void *a, const void *b) {
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

int compareByCodeLengthStabilized(const void *a, const void *b) {
    Element *element1 = (Element *) a;
    Element *element2 = (Element *) b;
    int length1 = strlen(element1->code);
    int length2 = strlen(element2->code);
    if (length1 < length2) {
        return -1;
    } else if (length1 > length2) {
        return 1;
    } else {  // if zero order by word
        return element1->word - element2->word;
    }
}

void orderByFreqDesc(Element *ptrElements) {
    qsort(ptrElements, MAX_CODE, sizeof(Element), compareByFreqStabilized);
}

void orderBycodeLengthCresc(Element *ptrElements) {
    qsort(ptrElements, MAX_CODE, sizeof(Element), compareByCodeLengthStabilized);
}

long sumFrequencies(Element *ptrElements, Node *root) {
    long sumOfFrequencies = 0;
    for (int i = root->start; i <= root->end; ++i) {
        sumOfFrequencies = sumOfFrequencies + ptrElements[i].frequency;
    }
    return sumOfFrequencies;
}


int getSplitIndex(Element *ptrElements, Node *root) {
    long sumOfFrequencies = sumFrequencies(ptrElements, root);
    int splitIndex = 0;
    double halfOfSum;
    long sum = 0;
    halfOfSum = 1.0 * sumOfFrequencies / 2;
    int from = root->start;
    int to = root->end;
    for (int i = from; i <= to; i++) {
        sum = sum + ptrElements[i].frequency;
        if (sum + ptrElements[i + 1].frequency >= halfOfSum) {
            splitIndex = i;
            return splitIndex;
        }

    }
    return splitIndex;
}


Node *createNode(Node *lNode, Node *rNode, int start, int end) {
    Node *newNode = (Node *) malloc(sizeof(Node));
    newNode->leftChild = lNode;
    newNode->rightChild = rNode;
    newNode->start = start;
    newNode->end = end;
    return newNode;
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


char charactersHolder[MAX_CODE] = "";

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


void writeCodesForAllElements(Element *ptrElements, Code *ptrCodes) {
    for (int i = 0; i < MAX_CODE; ++i) {
        int targetIndex = ptrElements[i].word;
        if (ptrElements[targetIndex].frequency > 0) {
            strcpy(ptrElements[targetIndex].code, ptrCodes[targetIndex].code);
            ptrElements[targetIndex].codeLength = (unsigned char) strlen(ptrElements[targetIndex].code);
        } else {
            ptrElements[targetIndex].codeLength = 0;
        }
    }
}

Code256 *getCode256() {
    Code256 *code256 = malloc(sizeof(Code256));
    code256->left = 0;
    code256->mid_left = 0;
    code256->mid_right = 0;
    code256->right = 0;
    return code256;
}


char *fromNumToChars(long long num, int length) {
    char *code = malloc(length * sizeof(char));
    strcpy(code, "");
    for (int i = length - 1; i >= 0; i--) {
        (num & (1 << i)) ? strcat(code, "1") : strcat(code, "0");
    }
    return code;

}

// funzionante per lunghezze di codifiche minori di 64 bits
void canonizeCodes(Element *ptrElements) {

    orderBycodeLengthCresc(ptrElements);
    Code256 *code256 = getCode256();
    int firstLength = ptrElements[0].codeLength;
    for (int i = 0; i < firstLength; ++i) {
        ptrElements[0].code[i] = '0';
    }
    int currentLength = 0;
    for (int i = 1; i < MAX_CODE; ++i) {
        currentLength = ptrElements[i].codeLength;
        if (currentLength == firstLength) {
            code256->right++;
            strcpy(ptrElements[i].code, fromNumToChars(code256->right, currentLength));
        } else if (currentLength > firstLength) {
            code256->right++;
            code256->right = code256->right << 1;
            strcpy(ptrElements[i].code, fromNumToChars(code256->right, currentLength));
        }
        firstLength = currentLength;
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

void writeLengths(FILE *outputFile, Element *ptrElements) {
    unsigned char lengths[MAX_CODE];
    for (int i = 0; i < MAX_CODE; ++i) {
        int targetIndex = ptrElements[i].word;
        lengths[targetIndex] = ptrElements[i].codeLength;
    }
    fwrite(lengths, sizeof(unsigned char), MAX_CODE, outputFile);
    bitsNumber += MAX_CODE;
}


void writeCompressedFile(FILE *inputFile, FILE *outputFile, Code *dict, Element *ptrElements) {
    int ch;
    fseek(inputFile, 0, SEEK_SET);
    writeLengths(outputFile, ptrElements);
    while ((ch = fgetc(inputFile)) != EOF) {
        int length = strlen(dict[ch].code);
        for (int i = 0; i < length; ++i) {
            addBit((unsigned char) dict[ch].code[i], outputFile);
        }
        bitsNumber += length;
    }
    fclose(outputFile);
}

void printCodes(Element *ptrElements) {
    for (int i = 0; i < MAX_CODE; ++i) {
        printf("code of %-4d length:%-4.d ", ptrElements[i].word, ptrElements[i].codeLength);
        for (int j = 0; ptrElements[i].code[j] != '\0'; ++j) {
            printf("%c", ptrElements[i].code[j]);
        }
        printf("\n");
    }
}

void compress() {
    Element *table = (Element *) malloc(MAX_CODE * sizeof(Element));
    Code *codes = (Code *) malloc(MAX_CODE * sizeof(Code));
    //Code *dictionary = (Code *) malloc(MAX_CODE * sizeof(Code));
    FILE *file = fopen("alice.txt", "rb");
    initializeTable(table);
    calculateFrequencies(file, table);
    printTable(table);
    orderByFreqDesc(table);

    printf("\nOrdered table(by frequencies):\n");
    printTable(table);
    Node *root = createNode(NULL, NULL, 0, MAX_CODE - 1);
    printf("\nSum of frequencies: %ld\n", sumFrequencies(table, root));

    createEncodingTree(table, root);
    encode(codes, root);
    writeCodesForAllElements(table, codes);
    printCodes(table);
    free(codes);

    canonizeCodes(table);

    printf("\ncodifica canonica:\n");
    printCodes(table);


    //FILE *compressed = fopen("compressed", "wb");

    //writeCompressedFile(file, compressed, codes, table);

    //printf("size of compressed file (in bits): %ld\n", bitsNumber);

}

