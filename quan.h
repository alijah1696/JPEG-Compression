/* quan.h
 * Alijah Jackson
 * CS 40, Project arith
 * 3/6/2025
 * This file contains function declarations for packing and 
 * unpacking pixels,constructing and deconstructing 
 * codewords, and handling chroma block
 * averages.
 */

#ifndef QUAN_H
#define QUAN_H

uint64_t* packPixels(const float *ycbcr, int width, int height);
uint64_t constructCodeword(unsigned a_quantized, int b_quantized, 
                           int c_quantized, int d_quantized, 
                           unsigned pb_index, unsigned pr_index);

float* unpackPixels(uint64_t* codewords, int width, int height);
float* deconstructCodeword(uint64_t codeword);
float* chromaBlockAverages(float block[][3]);
uint64_t to_little_endian(uint64_t word);

#endif