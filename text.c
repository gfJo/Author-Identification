#include "text.h"
#include "parser.h"
#include "ht.h"
#include "bf.h"

#include <regex.h>
#include <stdlib.h>
#include <math.h>

#define WORD "[a-zA-Z'-]+"

struct Text {
    HashTable *ht;
    BloomFilter *bf;
    uint32_t word_count;
};

uint32_t noiselimit;

Text *text_create(FILE *infile, Text *noise) {
    Text *text = (Text *) malloc(sizeof(Text)); // Create memory for text.
    if (text) {
        // Init ht and bf and word_count.
        text->ht = ht_create(pow(2, 19));
        text->bf = bf_create(pow(2, 21));
        text->word_count = 0;

        regex_t re;
        if (regcomp(&re, WORD, REG_EXTENDED)) {
            return (Text *) NULL;
        }

        // If there is no noise filter.
        if (!noise) {
            char *word = NULL;
            // Read all the words from the file and insert to ht and bf.
            uint32_t i = 0;
            while ((i < noiselimit) && ((word = next_word(infile, &re)) != NULL)) {
                ht_insert(text->ht, word);
                bf_insert(text->bf, word);
                text->word_count += 1;
                i += 1;
            }

        } else {
            // There is a noise filter.
            char *word = NULL;
            while ((word = next_word(infile, &re)) != NULL) {
                // Filter out the noise word.
                if ((!bf_probe(noise->bf, word)) || (!ht_lookup(noise->ht, word))) {
                    bf_insert(text->bf, word);
                    ht_insert(text->ht, word);
                    text->word_count += 1;
                }
            }
        }
        regfree(&re);
        return text;
    }
    return (Text *) NULL;
}

void text_delete(Text **text) {
    if (*text) {
        bf_delete(&(*text)->bf);
        ht_delete(&(*text)->ht);
        (*text)->word_count = 0;
        free(*text);
        *text = NULL;
    }
}

double text_dist(Text *text1, Text *text2, Metric metric) {
    double dist = 0;
    if (metric == MANHATTAN) {
        double sum = 0;
        Node *node = NULL;
        HashTableIterator *hti1 = hti_create(text1->ht);
        HashTableIterator *hti2 = hti_create(text2->ht);

        // Iter through text1 to calculate the dist.
        while ((node = ht_iter(hti1)) != NULL) {
            double freq1 = text_frequency(text1, node->word);
            double freq2 = text_frequency(text2, node->word);
            double min = freq1 - freq2;
            sum += fabs(min);
        }

        // Iter through text2.
        while ((node = ht_iter(hti2)) != NULL) {
            if (!text_contains(text1, node->word)) {
                sum += text_frequency(text2, node->word);
            }
        }

        hti_delete(&(hti1));
        hti_delete(&(hti2));
        dist = sum;
    } else if (metric == EUCLIDEAN) {
        double sum = 0;
        Node *node = NULL;
        HashTableIterator *hti1 = hti_create(text1->ht);
        HashTableIterator *hti2 = hti_create(text2->ht);

        // Iter through text1 to calculate the dist.
        while ((node = ht_iter(hti1)) != NULL) {
            double freq1 = text_frequency(text1, node->word);
            double freq2 = text_frequency(text2, node->word);
            double min = freq1 - freq2;
            double square = min * min;
            sum += square;
        }
        // Iter through text2.
        while ((node = ht_iter(hti2)) != NULL) {
            if (!text_contains(text1, node->word)) {
                double freq2 = text_frequency(text2, node->word);
                sum += (freq2 * freq2);
            }
        }

        hti_delete(&(hti1));
        hti_delete(&(hti2));
        dist = sqrt(sum);
    } else if (metric == COSINE) {
        double sum = 0;
        Node *node = NULL;
        HashTableIterator *hti1 = hti_create(text1->ht);

        // Iter through text1 to calculate the dist.
        while ((node = ht_iter(hti1)) != NULL) {
            double freq1 = text_frequency(text1, node->word);
            double freq2 = text_frequency(text2, node->word);
            double prod = freq1 * freq2;
            sum += prod;
        }
        hti_delete(&(hti1));
        dist = 1 - sum;
    }
    return dist;
}

double text_frequency(Text *text, char *word) {
    // Find in bloom filter first.
    if (bf_probe(text->bf, word)) {
        // Then find in hash table.
        Node *node = ht_lookup(text->ht, word);
        if (node) {
            double result = (double) node->count / (double) text->word_count;
            return result;
        }
    }
    return 0;
}

bool text_contains(Text *text, char *word) {
    if (bf_probe(text->bf, word)) {
        if (ht_lookup(text->ht, word)) {
            return true;
        }
    }
    return false;
}

void text_print(Text *text) {
    HashTableIterator *hti = hti_create(text->ht);
    Node *node;
    while ((node = ht_iter(hti)) != NULL) {
        printf("%s\n", node->word);
    }
    hti_delete(&hti);
}
