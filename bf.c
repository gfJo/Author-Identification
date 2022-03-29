#include "bf.h"
#include "salts.h"
#include "speck.h"

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

struct BloomFilter {
    uint64_t primary[2];
    uint64_t secondary[2];
    uint64_t tertiary[2];
    BitVector *filter;
};

BloomFilter *bf_create(uint32_t size) {
    // Create memory for Bloom Filters.
    BloomFilter *bf = (BloomFilter *) malloc(sizeof(BloomFilter));
    if (bf) {
        // Initialize all the salts.
        bf->primary[0] = SALT_PRIMARY_LO;
        bf->primary[1] = SALT_PRIMARY_HI;
        bf->secondary[0] = SALT_SECONDARY_LO;
        bf->secondary[1] = SALT_SECONDARY_HI;
        bf->tertiary[0] = SALT_SECONDARY_LO;
        bf->tertiary[1] = SALT_SECONDARY_HI;

        // Create memory for filter inside bf.
        bf->filter = bv_create(size);

        if (!bf->filter) {
            free(bf);
            bf = NULL;
        }
    }
    return bf;
}

void bf_delete(BloomFilter **bf) {
    if (*bf) {
        if ((*bf)->filter) {
            bv_delete(&(*bf)->filter);
        }
        free(*bf);
        *bf = NULL;
    }
}

uint32_t bf_size(BloomFilter *bf) {
    return bv_length(bf->filter);
}

void bf_insert(BloomFilter *bf, char *word) {
    // Get three indexes.
    uint32_t prm_index = hash(bf->primary, word) % bf_size(bf);
    uint32_t scd_index = hash(bf->secondary, word) % bf_size(bf);
    uint32_t ttr_index = hash(bf->tertiary, word) % bf_size(bf);

    // Insert three indexes into filters.
    bv_set_bit(bf->filter, prm_index);
    bv_set_bit(bf->filter, scd_index);
    bv_set_bit(bf->filter, ttr_index);
}

bool bf_probe(BloomFilter *bf, char *word) {
    // Get three indexes.
    uint32_t prm_index = hash(bf->primary, word) % bf_size(bf);
    uint32_t scd_index = hash(bf->secondary, word) % bf_size(bf);
    uint32_t ttr_index = hash(bf->tertiary, word) % bf_size(bf);

    // Get the bits in filter.
    bool prm_index_bit = bv_get_bit(bf->filter, prm_index);
    bool scd_index_bit = bv_get_bit(bf->filter, scd_index);
    bool ttr_index_bit = bv_get_bit(bf->filter, ttr_index);

    // Check if all three bits are set.
    if (prm_index_bit && scd_index_bit && ttr_index_bit) {
        return true;
    }
    return false;
}

void bf_print(BloomFilter *bf) {
    bv_print(bf->filter);
}
