/* compress40.c
 * Alijah Jackson
 * CS 40, Project arith
 * 3/6/2025
 * This file contains functions for compressing and decompressing images.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <reader.h>
#include <transforms.h>
#include <quan.h>

/*
 * name:      compress40
 * purpose:   Compresses a PPM image file by converting RGB values to YPbPr,
 *            packing the pixels into codewords, and printing the compressed
 *            data. This function reads the PPM image, transforms the color
 *            space, packs the pixels into codewords, and outputs the compressed
 *            image.
 * arguments: FILE *input - the input file pointer to the PPM image.
 * returns:   void
 * Author: Alijah Jackson
 */
void compress40(FILE *input){
        size_t size = 0;
        int width, height, maxVal;
        
        PPMData ppmdata = read_ppm(input, &size, &maxVal, &width, &height);

        float* chromaValues = rgb_to_ypbpr(ppmdata, width, height, maxVal);
        uint64_t* codewords = packPixels(chromaValues, width, height);

        printCompressed(codewords, width, height);
}

/*
 * name:      decompress40
 * purpose:   Decompresses a compressed image file by reading the compressed
 *            data, unpacking the codewords into YPbPr values, converting them
 *            back to RGB, and printing the PPM image. This function reads the
 *            compressed image, unpacks the codewords, transforms the color
 *            space back to RGB, and outputs the decompressed image.
 * arguments: FILE *input - the input file pointer to the compressed image.
 * returns:   void
 * Author: Alijah Jackson
 */
void decompress40(FILE *input){
        size_t size = 0;
        int width, height;

        uint64_t * codewords = read_compressed(input, &size, &width, &height);
        
        float * ypbpr = unpackPixels(codewords, width, height);
        PPMData ppmdata = ypbpr_to_rgb(ypbpr, width, height, PPM_max_value());

        print_ppm(ppmdata, width, height, PPM_max_value());
}