#include "bv.h"

#include <stdlib.h>
#include <stdio.h>

struct BitVector {
    uint32_t length;
    uint8_t *vector;
};

BitVector *bv_create(uint32_t length) {
    BitVector *bv = (BitVector *) malloc(sizeof(BitVector)); // Create memory for bv.
    if (bv) {
        bv->length = length;
        bv->vector = (uint8_t *) calloc(length, sizeof(uint8_t)); // Create memory for vectors.
        if (!(bv->vector)) { // If failed to create.
            bv = NULL; // Set the pointer to null and free the memory.
            free(bv);
        }
    }
    return bv;
}

void bv_delete(BitVector **bv) {
    if (*bv) { // If bv exists.
        if ((*bv)->vector) { // Free vector in bv if vector exists.
            free((*bv)->vector);
        }
        free(*bv);
        *bv = NULL; // Set the pointer to NULL.
    }
}

uint32_t bv_length(BitVector *bv) {
    return bv->length;
}

bool bv_set_bit(BitVector *bv, uint32_t i) {
    if (i < bv->length) {
        bv->vector[i / 8] |= (0x1 << (i % 8)); // Add 1 to bit.
        return true;
    }
    return false;
}

bool bv_clr_bit(BitVector *bv, uint32_t i) {
    if (i < bv->length) { // If i is within the range.
        bv->vector[i / 8] &= ~(0x1 << (i % 8)); // Set 0 to bit.
        return true;
    }
    return false;
}

bool bv_get_bit(BitVector *bv, uint32_t i) {
    // If and only if i in within the range and the bit is 1, return true.
    if (i < bv->length) {
        if (((bv->vector[i / 8] >> (i % 8)) & 1) == 1) {
            return true;
        }
    }
    return false;
}

void bv_print(BitVector *bv) {
    // Print all the bit in bv.
    if (bv) {
        for (uint32_t i = 0; i < bv->length; i += 1) {
            if (bv_get_bit(bv, i)) {
                printf("1 ");
            } else {
                printf("0 ");
            }
        }
        printf("\n");
    }
}
