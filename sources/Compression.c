#include <time.h>
#include "../headers/Compression.h"
#include "../headers/Decompression.h"

/************************ Compression's Functions Definition *****************************/

void initializeTable(Element *ptrElements) {
    for (int i = 0; i < MAX_CODE; ++i) {
        ptrElements[i].word = (unsigned char) i;
        ptrElements[i].frequency = 0;
        ptrElements[i].codeLength = 0;
    }
}


void fileIntoBuffer(FILE *inputFile, unsigned int fileSize, unsigned char *buffer) {
    fseek(inputFile, 0, SEEK_SET);
    fread(buffer, sizeof(unsigned char), fileSize, inputFile);
}


void calculateFrequencies(Element *ptrElements, unsigned char *buffer, int bufferSize) {
    unsigned char ch;
    int index = 0;
    while (index < bufferSize) {
        ch = buffer[index];
        ptrElements[ch].frequency++;
        index++;
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
    int length1 = element1->codeLength;
    int length2 = element2->codeLength;
    if (length1 < length2) {
        return -1;
    } else if (length1 > length2) {
        return 1;
    } else {  // if zero order by word
        return element1->word - element2->word;
    }
}


int compareByWord(const void *a, const void *b) {
    Element *element1 = (Element *) a;
    Element *element2 = (Element *) b;
    return element1->word - element2->word;
}


void orderByFreqDesc(Element *ptrElements) {
    qsort(ptrElements, MAX_CODE, sizeof(Element), compareByFreqStabilized);
}


void orderBycodeLengthCresc(Element *ptrElements) {
    qsort(ptrElements, MAX_CODE, sizeof(Element), compareByCodeLengthStabilized);
}


void orderByWord(Element *ptrElements) {
    qsort(ptrElements, MAX_CODE, sizeof(Element), compareByWord);
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
    for (int i = from; i < to; i++) {   // potrebbe servire  <= ?
        sum = sum + ptrElements[i].frequency;
        if (sum + ptrElements[i + 1].frequency >= halfOfSum) {
            splitIndex = i;
            return splitIndex;
        }
    }
    return splitIndex;
}


Node *createNode(Node *lNode, Node *rNode, int start, int end) {
    Node *newNode = malloc(sizeof(Node));
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
        }
    }
}


Code256Bits *getCode256Bits() {
    Code256Bits *code256 = malloc(sizeof(Code256Bits));
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

    Code256Bits *code256 = getCode256Bits();
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
            for (int j = 0; j < (currentLength - firstLength); ++j) {
                code256->right = code256->right << 1;
            }
            strcpy(ptrElements[i].code, fromNumToChars(code256->right, currentLength));
        }
        firstLength = currentLength;
    }
}


long bitsNumber = 0;    // All the bits written on the compressed file
unsigned char byte = 0;     // Single byte to write on the compressed file
int currentBit = 0;     // The current bit to add, when its value is 8, I reset it to 0

void addBit(unsigned char bit, FILE *file) {
    currentBit++;
    byte = byte << 1 | bit;
    if (currentBit == 8) {
        writeByte(file);
        currentBit = 0;
        byte = 0;
    }
}

void writeByte(FILE *file) {
    fputc(byte, file);
}


void writeLengths(FILE *outputFile, Element *ptrElements) {
    unsigned char lengths[MAX_CODE];
    for (int i = 0; i < MAX_CODE; ++i) {
        int targetIndex = ptrElements[i].word;
        lengths[targetIndex] = ptrElements[i].codeLength;
    }
    fwrite(lengths, sizeof(unsigned char), MAX_CODE, outputFile);
    bitsNumber += (MAX_CODE * 8);
}


void writeCompressedFile(unsigned char *buffer, int bufferSize, FILE *outputFile, Element *ptrElements) {

    fseek(outputFile, 1, SEEK_SET); // lascio spazio per il byte di flag!
    writeLengths(outputFile, ptrElements);

    fseek(outputFile, MAX_CODE + 1, SEEK_SET);

    unsigned char ch;
    int index = 0;
    while (index < bufferSize) {
        ch = buffer[index];
        for (int i = 0; i < ptrElements[ch].codeLength; ++i) {
            char bit = ptrElements[ch].code[i];
            bit == '0' ? addBit(0, outputFile) : addBit(1, outputFile);
        }
        bitsNumber += ptrElements[ch].codeLength;
        index++;
    }
    int lastBitsToWrite = 8 - (bitsNumber % 8);
    // completare l'ultimo byte con zeri
    for (int i = 0; i < lastBitsToWrite; ++i) {
        addBit(0, outputFile);
        bitsNumber++;
    }
    // scrivo il byte di flag
    fseek(outputFile, 0, SEEK_SET);
    fputc(lastBitsToWrite, outputFile);
    bitsNumber += 8;
    fclose(outputFile);
}

void printCodes(Element *ptrElements) {
    for (int i = 0; i < MAX_CODE; ++i) {
        printf("code of %-4d length: %-4d", ptrElements[i].word, ptrElements[i].codeLength);
        for (int j = 0; ptrElements[i].code[j] != '\0'; ++j) {
            printf("%c", ptrElements[i].code[j]);
        }
        printf("\n");
    }
}

void compress(char *toCompFileName, char *compFileName) {

    clock_t start = clock();

    Element *elements = malloc(MAX_CODE * sizeof(Element));
    Code *codes = malloc(MAX_CODE * sizeof(Code));

    FILE *toCompress = fopen(toCompFileName, "rb");
    initializeTable(elements);

    int bufferSize = getFileSize(toCompress);
    unsigned char *buffer = malloc(bufferSize * sizeof(unsigned char));
    fileIntoBuffer(toCompress, (unsigned int) bufferSize, buffer);

    calculateFrequencies(elements, buffer, bufferSize);
    fclose(toCompress);

    orderByFreqDesc(elements);
    //printCodes(elements);

    Node *root = createNode(NULL, NULL, 0, MAX_CODE - 1);
    createEncodingTree(elements, root);
    encode(codes, root);
    writeCodesForAllElements(elements, codes);
    free(codes);

    orderBycodeLengthCresc(elements);
    canonizeCodes(elements);

    orderByWord(elements);

    FILE *compressed = fopen(compFileName, "wb");
    writeCompressedFile(buffer, bufferSize, compressed, elements);

    clock_t end = clock();
    double compTime = (double) (end - start) / CLOCKS_PER_SEC;
    printf("Compression's time: %.5lf seconds\n", compTime);

}

