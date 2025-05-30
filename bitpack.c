/* bitpack.c
 * Alijah Jackson
 * CS 40, Project arith
 * 3/6/2025
 * This file contains functions for packing and unpacking bits.
 */

#include "bitpack.h"
#include <assert.h>
#include <stdio.h>

Except_T Bitpack_Overflow = { "Overflow packing bits" };

/*
 * name:      check_width
 * purpose:   Ensure the width does not exceed 64 bits
 * arguments: unsigned width - the width to check
 * returns:   void
 * Author: Alijah Jackson
 */
static inline void check_width(unsigned width) {
        assert(width <= 64);
}

/*
 * name:      Bitpack_fitsu
 * purpose:   Check if an unsigned integer fits within the specified width
 * arguments: uint64_t n - the number to check
 *            unsigned width - the width to fit within
 * returns:   bool - true if the number fits, false otherwise
 * Author: Alijah Jackson
 */
bool Bitpack_fitsu(uint64_t n, unsigned width) {
        check_width(width);
        return (width == 0 || n < (1ULL << width));
}

/*
 * name:      Bitpack_fitss
 * purpose:   Check if a signed integer fits within the specified width
 * arguments: int64_t n - the number to check
 *            unsigned width - the width to fit within
 * returns:   bool - true if the number fits, false otherwise
 * Author: Alijah Jackson
 */
bool Bitpack_fitss(int64_t n, unsigned width) {
        check_width(width);
        if (width == 0) return false;
        int64_t min = -(1LL << (width - 1));
        int64_t max = (1LL << (width - 1)) - 1;
        return (n >= min && n <= max);
}

/*
 * name:      Bitpack_getu
 * purpose:   Extract an unsigned value from a word
 * arguments: uint64_t word - the word to extract from
 *            unsigned width - the width of the value to extract
 *            unsigned lsb - the least significant bit of the value
 * returns:   uint64_t - the extracted value
 * Author: Alijah Jackson
 */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb) {
        check_width(width);
        assert(width + lsb <= 64);
        return (word >> lsb) & ((1ULL << width) - 1);
}

/*
 * name:      Bitpack_gets
 * purpose:   Extract a signed value from a word
 * arguments: uint64_t word - the word to extract from
 *            unsigned width - the width of the value to extract
 *            unsigned lsb - the least significant bit of the value
 * returns:   int64_t - the extracted value
 * Author: Alijah Jackson
 */
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb) {
        uint64_t mask = (1ULL << width) - 1;
        int64_t value = (word >> lsb) & mask;
        if (value & (1ULL << (width - 1))) {
                value -= (1ULL << width);
        }
        return value;
}

/*
 * name:      Bitpack_newu
 * purpose:   Insert an unsigned value into a word
 * arguments: uint64_t word - the word to insert into
 *            unsigned width - the width of the value to insert
 *            unsigned lsb - the least significant bit of the value
 *            uint64_t value - the value to insert
 * returns:   uint64_t - the word with the inserted value
 * Author: Alijah Jackson
 */
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb,
                      uint64_t value) {
        check_width(width);
        assert(width + lsb <= 64);
        if (!Bitpack_fitsu(value, width)) {
                RAISE(Bitpack_Overflow);
        }
        uint64_t mask = ((1ULL << width) - 1) << lsb;
        return (word & ~mask) | ((value << lsb) & mask);
}

/*
 * name:      Bitpack_news
 * purpose:   Insert a signed value into a word
 * arguments: uint64_t word - the word to insert into
 *            unsigned width - the width of the value to insert
 *            unsigned lsb - the least significant bit of the value
 *            int64_t value - the value to insert
 * returns:   uint64_t - the word with the inserted value
 * Author: Alijah Jackson
 */
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,
                      int64_t value) {
        check_width(width);
        assert(width + lsb <= 64);
        if (!Bitpack_fitss(value, width)) {
                RAISE(Bitpack_Overflow);
        }
        uint64_t mask = ((1ULL << width) - 1) << lsb;
        uint64_t uvalue = (uint64_t)value & ((1ULL << width) - 1);
        return (word & ~mask) | ((uvalue << lsb) & mask);
}