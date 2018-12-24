
#include "../headers/Decompression.h"
#include "../headers/Compression.h"

int lastbits;

/************************ Decompression's Functions Definition *****************************/

int getFileSize(FILE *inputFile) {
    fseek(inputFile, 0, SEEK_END);
    int size = ftell(inputFile);
    return size;
}

// header include 1 byte di flag e 256 bytes di lunghezze
void readHeader(FILE *inputfile, Element *ptrElements) {
    unsigned char lengths[MAX_CODE];
    fseek(inputfile, 0, SEEK_SET);
    fread(&lastbits, sizeof(unsigned char), 1, inputfile);
    fread(lengths, sizeof(unsigned char), MAX_CODE, inputfile);

    for (int i = 0; i < MAX_CODE; ++i) {
        ptrElements[i].word = (unsigned char) i;
        ptrElements[i].codeLength = lengths[i];
    }
}


void readCompressedFile(FILE *inputFile, unsigned int bufferSize, unsigned char *buffer) {
    fseek(inputFile, MAX_CODE + 1, SEEK_SET);
    fread(buffer, sizeof(unsigned char), bufferSize, inputFile);

}


void createDecodingTree(Node *root, const char *code, int index, unsigned char word) {
    if (code[index] != '\0') {
        if (code[index] == '0') {
            if (root->leftChild == NULL) {
                Node *lNode = createNode(NULL, NULL, -1, -1);
                root->leftChild = lNode;
                index++;
                createDecodingTree(root->leftChild, code, index, word);
            } else {
                index++;
                createDecodingTree(root->leftChild, code, index, word);
            }
        } else {
            if (root->rightChild == NULL) {
                Node *rNode = createNode(NULL, NULL, -1, -1);
                root->rightChild = rNode;
                index++;
                createDecodingTree(root->rightChild, code, index, word);
            } else {
                index++;
                createDecodingTree(root->rightChild, code, index, word);
            }
        }
    } else {
        root->start = word;
        root->end = word;
        return;
    }
}


void decode(Node *root, Element *ptrElements) {
    for (int i = 0; i < MAX_CODE; ++i) {
        if (ptrElements[i].codeLength != 0) {
            createDecodingTree(root, ptrElements[i].code, 0, ptrElements[i].word);
        }
    }
}


// 1 <= bit position <= 8
int extractBitOnPosition(unsigned char byte, int bitPosition) {
    return (byte & (1 << (8 - bitPosition)) ? '1' : '0');
}

char *byteToChars(unsigned char byte) {
    char *chars = malloc(8 * sizeof(char));
    for (int i = 1; i <= 8; ++i) {
        chars[i - 1] = extractBitOnPosition(byte, i);
    }
    return chars;
}


void writeDecompressedFile(FILE *outputFile, Node *root, unsigned char *buffer, int bufferSize) {

    Node *actualRoot = root;
    int index = 0;

    while (index < bufferSize) {
        char *byte = byteToChars(buffer[index]);

        for (int i = 0; i < 8; ++i) {
            if (index == bufferSize - 1 && i == (8 - lastbits)) {
                break;
            }
            if (byte[i] == '0')
                root = root->leftChild;
            else if (byte[i] == '1')
                root = root->rightChild;
            if (root->leftChild == NULL && root->rightChild == NULL) {
                fputc(root->start, outputFile);
                root = actualRoot;
            }
        }
        index++;
    }

}


void decompress() {

    Element *elements = malloc(MAX_CODE * sizeof(Element));
    Node *root = createNode(NULL, NULL, -1, -1);
    FILE *compressed = fopen("compressed", "rb");

    int fileSize = getFileSize(compressed);
    int bufferSize = fileSize - (MAX_CODE + 1);   // size of header = MAX_CODE + 1 = 257


    readHeader(compressed, elements);

    orderBycodeLengthCresc(elements);
    canonizeCodes(elements);

    //printCodes(elements);

    unsigned char *buffer = malloc(bufferSize * sizeof(unsigned char));
    readCompressedFile(compressed, (unsigned int) bufferSize, buffer);
    decode(root, elements);

    FILE *decompressed = fopen("decompressed", "wb");
    writeDecompressedFile(decompressed, root, buffer, bufferSize);
    fclose(decompressed);


    printf("\n%d", lastbits);


}
