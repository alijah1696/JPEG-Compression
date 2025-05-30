/* reader.c
 * Alijah Jackson
 * CS 40, Project arith
 * 3/6/2025
 * This file contains functions for reading and printing PPM and compressed
 * image data.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include <reader.h>
#include <quan.h>
#include <arith40.h>

char COMPRESSED_HEADER[] = "COMP40 Compressed image format 2";
int PPM_MAX_VAL = 25;

/*
******************************  PROTOTYPE FUNCTIONS ************************
*/

PPMData trim_ppm(PPMData original_data, int *width, int *height);

/*
******************************  MAIN FUNCTIONS ************************
*/

/*
 * name:      read_ppm
 * purpose:   Reads a PPM image from a file and returns the image data.
 * arguments: FILE *fp - file pointer to the PPM file
 *            size_t *size - pointer to store the size of the image data
 *            int *maxval - pointer to store the maximum pixel value
 *            int *width - pointer to store the width of the image
 *            int *height - pointer to store the height of the image
 * returns:   PPMData - pointer to the image data
 * Author: Alijah Jackson
 */
PPMData read_ppm(FILE *fp, size_t *size, int *maxval, int *width, int *height) {
        char header[64];

        fgets(header, 64, fp);
        fscanf(fp, "%d %d %d", width, height, maxval);
        fgetc(fp);

        *size = 3 * (*width) * (*height);
        PPMData data = (PPMData)malloc(*size);
        if (data == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                return NULL;
        }
        if (fread(data, 1, *size, fp) != *size) {
                fprintf(stderr, "Error reading pixel data\n");
                free(data);
                return NULL;
        }

        return trim_ppm(data, width, height);
}

/*
 * name:      print_ppm
 * purpose:   Prints a PPM image to stdout.
 * arguments: const PPMData data - pointer to the image data
 *            int width - width of the image
 *            int height - height of the image
 *            int maxVal - maximum pixel value
 * returns:   void
 * Author: Alijah Jackson
 */
void print_ppm(const PPMData data, int width, int height, int maxVal) {
        if (data == NULL) {
                fprintf(stderr, "No image data to print.\n");
                return;
        }
        printf("P6\n");
        printf("%d %d\n%d\n", width, height, maxVal);
        fwrite(data, 1, width * height * 3, stdout);
}

/*
 * name:      read_compressed
 * purpose:   Reads compressed image data from a file.
 * arguments: FILE *p - file pointer to the compressed file
 *            size_t *size - pointer to store the size of the compressed data
 *            int *width - pointer to store the width of the image
 *            int *height - pointer to store the height of the image
 * returns:   CompressedData - pointer to the compressed image data
 * Author: Alijah Jackson
 */
CompressedData read_compressed(FILE *p, size_t *size, int *width, int *height) {
        char header[40];
        assert(fgets(header, sizeof(header), p) != NULL);
        assert(strcmp(header, "COMP40 Compressed image format 2\n") == 0);
        assert(fscanf(p, "%d %d", width, height) == 2);

        int c;
        while ((c = fgetc(p)) != EOF && c != '\n');

        *size = ((*width) * (*height)) / 4;
        CompressedData compressedData = malloc(*size * sizeof(uint64_t));
        assert(compressedData != NULL);

        size_t read_count = fread(compressedData, sizeof(uint64_t), *size, p);
        assert(read_count == *size);

        for (size_t i = 0; i < read_count; i++) {
                compressedData[i] = to_little_endian(compressedData[i]);
        }

        return compressedData;
}

/*
 * name:      printCompressed
 * purpose:   Prints compressed image data to stdout.
 * arguments: uint64_t *codewords - pointer to the compressed image data
 *            int width - width of the image
 *            int height - height of the image
 * returns:   void
 * Author: Alijah Jackson
 */
void printCompressed(uint64_t *codewords, int width, int height) {
        if (codewords == NULL) {
                fprintf(stderr, "No image data to print.\n");
                return;
        }
        printf("%s\n%u %u\n", COMPRESSED_HEADER, width, height);
        int total_blocks = (width * height) / 4;
        for (int i = 0; i < total_blocks; i++) {
                uint64_t codeword = codewords[i];
                for (int shift = 56; shift >= 0; shift -= 8) {
                        putchar((codeword >> shift) & 0xFF);
                }
        }
}

/*
************************  HELPER FUNCTIONS ****************************
*/

/*
 * name:      trim_ppm
 * purpose:   Trims the PPM image data to ensure dimensions are even.
 * arguments: PPMData original_data - pointer to the original image data
 *            int *width - pointer to the width of the image
 *            int *height - pointer to the height of the image
 * returns:   PPMData - pointer to the trimmed image data
 * Author: Alijah Jackson
 */
PPMData trim_ppm(PPMData original_data, int *width, int *height) {
        int original_width = *width;
        int original_height = *height;

        int new_width = original_width - (original_width % 2);
        int new_height = original_height - (original_height % 2);

        size_t new_size = 3 * new_width * new_height;
        PPMData new_data = (PPMData)malloc(new_size);

        for (int row = 0; row < new_height; row++) {
                for (int col = 0; col < new_width; col++) {
                        size_t orig_idx = (row * original_width + col) * 3;
                        size_t new_idx = (row * new_width + col) * 3;
                        memcpy(&new_data[new_idx], &original_data[orig_idx], 3);
                }
        }

        *width = new_width;
        *height = new_height;
        return new_data;
}

/*
 * name:      free_image
 * purpose:   Frees the allocated memory for image data.
 * arguments: unsigned char *data - pointer to the image data
 * returns:   void
 * Author: Alijah Jackson
 */
void free_image(unsigned char *data) {
        free(data);
}

/*
 * name:      PPM_max_value
 * purpose:   Returns the maximum pixel value for PPM images.
 * arguments: void
 * returns:   int - maximum pixel value
 * Author: Alijah Jackson
 */
int PPM_max_value() {
        return PPM_MAX_VAL;
}