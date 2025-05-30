/* reader.h
 * Alijah Jackson
 * CS 40, Project arith
 * 3/6/2025
 * This file contains function declarations for reading and writing PPM and 
 * compressed image files.
 */

#ifndef READER_H
#define READER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char *PPMData;
typedef uint64_t *CompressedData;

PPMData read_ppm(FILE *fp, size_t *size, int *maxval, int *width, int *height);
void print_ppm(const PPMData data, int width, int height, int maxVal);
PPMData trim_ppm(PPMData original_data, int *width, int *height);
CompressedData read_compressed(FILE *p, size_t *size, int *width, int *height);
void printCompressed(uint64_t *codewords, int width, int height);
void debugPPM(PPMData data, int width, int height);
void free_image(unsigned char *data);
int PPM_max_value();

#endif