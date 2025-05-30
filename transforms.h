/* transforms.h
 * Alijah Jackson
 * CS 40, Project arith
 * 3/6/2025
 * This file contains function declarations for transforming 
 * image data between different color spaces and performing 
 * various pixel operations.
 */

#ifndef TRANSFORMS_H
#define TRANSFORMS_H

#include <stdint.h>
#include <stdlib.h>

float* rgb_to_ypbpr(const unsigned char *rgb, int width, 
                    int height, int maxVal);
unsigned char* ypbpr_to_rgb(const float *ypbpr, int width, 
                            int height, int maxVal);
float* pixelsToCoefficients(float pixels[4]);
float* coefficientsToPixels(float coefs[4]);
float clamp(float value, float min, float max);
float roundf(float x);

#endif // TRANSFORMS_H
