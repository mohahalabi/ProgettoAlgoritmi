cmake_minimum_required(VERSION 3.12)
project(Compressor C)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES Main.c Structures.h CompressionFunctions.h)

add_executable(Compressor ${SOURCE_FILES})