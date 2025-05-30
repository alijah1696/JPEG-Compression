/* transforms.c
 * Alijah Jackson
 * CS 40, Project arith
 * 3/6/2025
 * This file contains functions for transforming image data between
 * different color spaces and performing various pixel operations.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

float clamp(float value, float min, float max);
float roundf(float x);

/*
 * name:      rgb_to_ypbpr
 * purpose:   Convert RGB image data to YPbPr color space
 * arguments: const unsigned char *rgb - input RGB image data
 *            int width - width of the image
 *            int height - height of the image
 *            int maxVal - maximum value for RGB components
 * returns:   float* - YPbPr image data
 * Author: Alijah Jackson
 */
float* rgb_to_ypbpr(const unsigned char *rgb, int width, int height, 
                    int maxVal) {
    float* ypbpr = malloc(3 * width * height * sizeof(float));
    for (int i = 0; i < width * height; i++) {
        float r = (float) rgb[3*i] / maxVal;
        float g = (float) rgb[3*i+1] / maxVal;
        float b = (float) rgb[3*i+2] / maxVal;

        ypbpr[3*i]   = 0.299f * r + 0.587f * g + 0.114f * b;
        ypbpr[3*i+1] = -0.168736f * r - 0.331264f * g + 0.5f * b;
        ypbpr[3*i+2] = 0.5f * r - 0.418688f * g - 0.081312f * b;
    }
    return ypbpr;
}

/*
 * name:      ypbpr_to_rgb
 * purpose:   Convert YPbPr image data to RGB color space
 * arguments: const float *ypbpr - input YPbPr image data
 *            int width - width of the image
 *            int height - height of the image
 *            int maxVal - maximum value for RGB components
 * returns:   unsigned char* - RGB image data
 * Author: Alijah Jackson
 */
unsigned char* ypbpr_to_rgb(const float *ypbpr, int width, int height, 
                            int maxVal) {
    unsigned char* rgb = malloc(3 * width * height * sizeof(unsigned char));

    for (int i = 0; i < width * height; i++) {
        float r = clamp((ypbpr[3*i] + 1.402f * ypbpr[3*i+2]), 0, 1);
        float g = clamp((ypbpr[3*i] - 0.344136f * ypbpr[3*i+1] - 
                        0.714136f * ypbpr[3*i+2]), 0, 1);
        float b = clamp((ypbpr[3*i] + 1.772f * ypbpr[3*i+1]), 0, 1);

        rgb[3*i] = (unsigned char) roundf(r * maxVal);
        rgb[3*i+1] = (unsigned char) roundf(g * maxVal);
        rgb[3*i+2] = (unsigned char) roundf(b * maxVal);
    }
    return rgb;
}

/*
 * name:      pixelsToCoefficients
 * purpose:   Convert pixel values to DCT coefficients
 * arguments: float pixels[4] - array of 4 pixel values
 * returns:   float* - array of 4 DCT coefficients
 * Author: Alijah Jackson
 */
float* pixelsToCoefficients(float pixels[4]) {
    const float Y1 = pixels[0]; 
    const float Y2 = pixels[1];
    const float Y3 = pixels[2]; 
    const float Y4 = pixels[3]; 
    
    float* coefs = malloc(4 * sizeof(float));

    coefs[0] = (Y4 + Y3 + Y2 + Y1) / 4.0f;  // a (DC coefficient)
    coefs[1] = (Y4 + Y3 - Y2 - Y1) / 4.0f;  // b (vertical gradient)
    coefs[2] = (Y4 - Y3 + Y2 - Y1) / 4.0f;  // c (horizontal gradient)
    coefs[3] = (Y4 - Y3 - Y2 + Y1) / 4.0f;  // d (diagonal gradient)

    return coefs;
}

/*
 * name:      coefficientsToPixels
 * purpose:   Convert DCT coefficients to pixel values
 * arguments: float coefs[4] - array of 4 DCT coefficients
 * returns:   float* - array of 4 pixel values
 * Author: Alijah Jackson
 */
float* coefficientsToPixels(float coefs[4]) {
    const float a = coefs[0];
    const float b = coefs[1];
    const float c = coefs[2];
    const float d = coefs[3];

    float* pixels = malloc(4 * sizeof(float));

    pixels[0] = a - b - c + d;  // Y1 (top-left)
    pixels[1] = a - b + c - d;  // Y2 (top-right)
    pixels[2] = a + b - c - d;  // Y3 (bottom-left)
    pixels[3] = a + b + c + d;  // Y4 (bottom-right)

    return pixels;
}

/*
 * name:      clamp
 * purpose:   Clamp a value between a minimum and maximum
 * arguments: float value - the value to clamp
 *            float min - the minimum value
 *            float max - the maximum value
 * returns:   float - the clamped value
 * Author: Alijah Jackson
 */
float clamp(float value, float min, float max) {
    return (value < min) ? min : (value > max) ? max : value;
}

/*
 * name:      roundf
 * purpose:   Round a floating-point value to the nearest integer
 * arguments: float x - the value to round
 * returns:   float - the rounded value
 * Author: Alijah Jackson
 */
float roundf(float x) {
    return (x >= 0.0f) ? (float)(int)(x + 0.5f) : (float)(int)(x - 0.5f);
}