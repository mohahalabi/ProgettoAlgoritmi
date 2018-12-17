
#ifndef COMPRESSOR_DECOMPRESSIONFUNCTIONS_H
#define COMPRESSOR_DECOMPRESSIONFUNCTIONS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "Compression.h"


/********************** Functions Declaration ******************************/

int getFileSize(FILE *inputFile);

void readLengths(FILE *inputfile, Element *ptrElements);


#endif //COMPRESSOR_DECOMPRESSIONFUNCTIONS_H
