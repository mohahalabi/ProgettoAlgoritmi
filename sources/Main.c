/*
Shannon-Fano implementation.
****************************************************************************
Author: Mohammad Halabi
Programming language: C standard version 99
This is a prototype implementation which will be developed during the
third semester as a project of algorithms and data structures course.

****************************************************************************
Byte di flag: indica quanti bit sono stati aggiunti alla fine del file compresso
*/

#include <stdio.h>
#include <string.h>
#include "../headers/Compression.h"
#include "../headers/Decompression.h"



int main() {

    compress();
    decompress();

    return 0;
}
