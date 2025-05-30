#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arith40.h>
#include <bitpack.h> 
#include <transforms.h>

#define DEFAULT_SIZE 2

/*
******************************  PROTOTYPE FUNCTIONS ************************
*/

float* chromaBlockAverages(float block[][3]);
uint64_t constructCodeword(unsigned a_quantized, int b_quantized, 
                           int c_quantized, int d_quantized, 
                           unsigned pb_index, unsigned pr_index);
float* deconstructCodeword(uint64_t codeword);

/*
******************************  MAIN FUNCTIONS ************************
*/

/*
 * name:      packPixels
 * purpose:   Packs pixel values into a 64-bit codeword.
 *            This function takes individual pixel components and combines
 *            them into a single 64-bit codeword using bit manipulation
 *            techniques. The packed codeword can be used for efficient
 *            storage and transmission of pixel data.
 * arguments: float y, float pb, float pr - The Y, Pb, and Pr components
 *                                          of the pixel to be packed.
 * returns:   uint64_t - The packed 64-bit codeword containing the pixel
 *                       data.
 * Author: Alijah Jackson
 */
uint64_t* packPixels(const float *ycbcr, int width, int height) {
    int block_width = width / 2;
    int block_number = (width * height) / 4;
    uint64_t *codewords = malloc(block_number * sizeof(uint64_t));

    for (int block_idx = 0; block_idx < block_number; block_idx++) {
        int block_row = (block_idx / block_width) * 2;
        int block_col = (block_idx % block_width) * 2;
        int base_idx = (block_row * width + block_col) * 3;

        float block[4][3];
        for (int i = 0; i < 4; i++) {
            int offset_row = (i / 2) * width * 3;
            int offset_col = (i % 2) * 3;
            int data_idx = base_idx + offset_row + offset_col;
            block[i][0] = ycbcr[data_idx];
            block[i][1] = ycbcr[data_idx + 1];
            block[i][2] = ycbcr[data_idx + 2];
        }

        float* chroma_avg = chromaBlockAverages(block);

        float y_pixels[4] = { block[0][0], block[1][0], block[2][0], 
                              block[3][0] };
        float* coefficients = pixelsToCoefficients(y_pixels);

        unsigned a = (unsigned)roundf(coefficients[0] * 511.0f);
        a = (a > 511) ? 511 : a;
        int b = clamp((int)roundf(coefficients[1] / 0.3f * 15.0f), -15, 15);
        int c = clamp((int)roundf(coefficients[2] / 0.3f * 15.0f), -15, 15);
        int d = clamp((int)roundf(coefficients[3] / 0.3f * 15.0f), -15, 15);
        unsigned pb_index = Arith40_index_of_chroma(chroma_avg[0]);
        unsigned pr_index = Arith40_index_of_chroma(chroma_avg[1]);
    
        codewords[block_idx] = constructCodeword(a, b, c, d, pb_index, 
                                                 pr_index);
    }
    return codewords;
}

/*
 * name:      unpackPixels
 * purpose:   Unpacks pixel values from a 64-bit codeword.
 *            This function extracts individual pixel components from a
 *            64-bit codeword using bit manipulation techniques. The
 *            unpacked values are returned as an array of floating-point
 *            numbers.
 * arguments: uint64_t codeword - The 64-bit codeword containing the
 *                                packed pixel data.
 * returns:   float* - A dynamically allocated array of floating-point
 *                     values representing the unpacked pixel components.
 *                     The array contains three elements: Y, Pb, and Pr.
 * Author: Alijah Jackson
 */
float* unpackPixels(uint64_t* codewords, int width, int height) {
    int numBlocks = (width * height) / 4;
    int size = width * height * 3;

    float* ypbpr = malloc(size * sizeof(float));
    for (int block_idx = 0; block_idx < numBlocks; block_idx++) {
        float* qn = deconstructCodeword(codewords[block_idx]);

        float coefs[4] = { qn[0], qn[1], qn[2], qn[3] };
        float* pixels = coefficientsToPixels(coefs);

        float pb = qn[4];
        float pr = qn[5];

        for (int i = 0; i < 4; i++) {
            int row = (block_idx / (width / 2)) * 2 + (i / 2);
            int col = (block_idx % (width / 2)) * 2 + (i % 2);
            int base_idx = (row * width + col) * 3;

            ypbpr[base_idx] = pixels[i];
            ypbpr[base_idx + 1] = pb;
            ypbpr[base_idx + 2] = pr;
        }

        free(qn);
        free(pixels);
    }
    return ypbpr;
}

/*
******************************  HELPER FUNCTIONS **************************
*/

/*
 * name:      constructCodeword
 * purpose:   Constructs a codeword from quantized values
 * arguments: quantized values of a,b,c,d and the pb and pr indices
 * returns:   uint64_t - constructed codeword
 * Author: Alijah Jackson
 */
uint64_t constructCodeword(unsigned a_quantized, int b_quantized, 
                           int c_quantized, int d_quantized, 
                           unsigned pb_index, unsigned pr_index) {
    uint64_t codeword = 0;
    codeword = Bitpack_newu(codeword, 9, 23, a_quantized);
    codeword = Bitpack_news(codeword, 5, 18, b_quantized);
    codeword = Bitpack_news(codeword, 5, 13, c_quantized);
    codeword = Bitpack_news(codeword, 5, 8, d_quantized);
    codeword = Bitpack_newu(codeword, 4, 4, pb_index);
    codeword = Bitpack_newu(codeword, 4, 0, pr_index);
    return codeword;
}

/*
 * name:      deconstructCodeword
 * purpose:   Deconstructs a codeword into quantized values
 * arguments: uint64_t codeword - codeword to be deconstructed
 * returns:   float* - array of deconstructed values
 * Author: Alijah Jackson
 */
float* deconstructCodeword(uint64_t codeword) {
    float* values = malloc(6 * sizeof(float));
    if (!values) return NULL;

    values[0] = ((unsigned) Bitpack_getu(codeword, 9, 23)) / 511.0f;
    values[1] = ((int) Bitpack_gets(codeword, 5, 18)) * 0.3f / 15.0f;
    values[2] = ((int) Bitpack_gets(codeword, 5, 13)) * 0.3f / 15.0f;
    values[3] = ((int) Bitpack_gets(codeword, 5, 8)) * 0.3f / 15.0f;
    values[4] = Arith40_chroma_of_index((unsigned) Bitpack_getu(codeword, 
                                                                4, 4));
    values[5] = Arith40_chroma_of_index((unsigned) Bitpack_getu(codeword, 
                                                                4, 0));
    return values;
}

/*
 * name:      to_little_endian
 * purpose:   Converts a 64-bit word to little-endian format
 * arguments: uint64_t word - word to be converted
 * returns:   uint64_t - converted word in little-endian format
 * Author: Alijah Jackson
 */
uint64_t to_little_endian(uint64_t word) {
    return ((word >> 56) & 0x00000000000000FFULL) |
           ((word >> 40) & 0x000000000000FF00ULL) |
           ((word >> 24) & 0x0000000000FF0000ULL) |
           ((word >> 8) & 0x00000000FF000000ULL) |
           ((word << 8) & 0x000000FF00000000ULL) |
           ((word << 24) & 0x0000FF0000000000ULL) |
           ((word << 40) & 0x00FF000000000000ULL) |
           ((word << 56) & 0xFF00000000000000ULL);
}

/*
 * name:      chromaBlockAverages
 * purpose:   Computes the average chroma values for a 2x2 block
 * arguments: float block[][3] - 2x2 block of YCbCr values
 * returns:   float* - array containing average Cb and Cr values
 * Author: Alijah Jackson
 */
float* chromaBlockAverages(float block[][3]) {
    float* chroma = malloc(2 * sizeof(float));
    float total_cb = 0.0f, total_cr = 0.0f;
    for (int i = 0; i < 4; i++) {
        total_cb += block[i][1];
        total_cr += block[i][2];
    }
    chroma[0] = clamp((total_cb / 4.0f), -0.5f, 0.5f);
    chroma[1] = clamp((total_cr / 4.0f), -0.5f, 0.5f);
    return chroma;
}