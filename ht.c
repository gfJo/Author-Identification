#include "ht.h"
#include "salts.h"
#include "speck.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

struct HashTable {
    uint64_t salt[2];
    uint32_t size;
    Node **slots;
};

struct HashTableIterator {
    HashTable *table;
    uint32_t slot;
};

HashTable *ht_create(uint32_t size) {
    HashTable *ht = (HashTable *) malloc(sizeof(HashTable)); // Create a hashtable.
    if (ht) { // If create successfully.
        ht->salt[0] = SALT_HASHTABLE_LO;
        ht->salt[1] = SALT_HASHTABLE_HI;
        ht->size = size;
        ht->slots = (Node **) calloc(size, sizeof(Node *)); // Create memory for slots.

        for (uint32_t i = 0; i < size; i += 1) { // Set all the pointer to NULL.
            ht->slots[i] = NULL;
        }

        if (!ht->slots) { // If failed to create, then free the memory.
            free(ht);
            ht = NULL;
        }
    }
    return ht;
}

void ht_delete(HashTable **ht) {
    if (*ht) { // If hashtable exist.
        if ((*ht)->slots) { // Continue to check the slots.
            for (uint32_t i = 0; i < (*ht)->size; i += 1) { // Free all slots in the ht.
                node_delete(&((*ht)->slots[i]));
            }
            free((*ht)->slots); // Free the slots.
        }
        (*ht)->size = 0;
        free(*ht);
        *ht = NULL;
    }
}

uint32_t ht_size(HashTable *ht) {
    return ht->size;
}

Node *ht_lookup(HashTable *ht, char *word) {
    // Get the index of the of the word.
    uint32_t index = hash(ht->salt, word) % ht->size;
    uint32_t count = 0;

    // Check throught the array of slots.
    while (count < ht->size) {
        Node *node = ht->slots[index];
        // If found, return the pointer of the node.
        if (node && (strcmp(node->word, word) == 0)) {
            return node;
        }
        // Check next one.
        index = (index + 1) % ht->size;
        count += 1;
    }
    // Not found.
    return (Node *) NULL;
}

Node *ht_insert(HashTable *ht, char *word) {
    // Get the index.
    uint32_t index = hash(ht->salt, word) % ht->size;
    uint32_t count = 0;

    while (count < ht->size) {
        Node *node = ht->slots[index];
        if (node && (strcmp(node->word, word) == 0)) {
            // If the word exists in the hash table.
            ht->slots[index]->count += 1;
            return node;
        } else if (node == NULL) {
            // The word doesn't exists.
            node = node_create(word); // Create the node.
            node->count = 1; // Set the count to 1.
            ht->slots[index] = node; // Insert it to hashtable.
            return node;
        }
        // Check next one.
        count += 1;
        index = (index + 1) % ht->size;
    }
    // The hashtable is full.
    return (Node *) NULL;
}

void ht_print(HashTable *ht) {
    for (uint32_t i = 0; i < ht->size; i += 1) {
        if (ht->slots[i]) {
            node_print(ht->slots[i]);
        }
    }
    printf("\n");
}

HashTableIterator *hti_create(HashTable *ht) {
    HashTableIterator *hti = (HashTableIterator *) malloc(sizeof(HashTableIterator));
    if (hti) {
        hti->table = ht;
        hti->slot = 0;
    }
    return hti;
}

void hti_delete(HashTableIterator **hti) {
    if (*hti) {
        free(*hti);
        *hti = NULL;
    }
}

Node *ht_iter(HashTableIterator *hti) {
    // Iterate over the hashtable.
    while (hti->slot < hti->table->size) {
        // Get the index of the iterators and the node at that index.
        uint32_t index = hti->slot;
        Node *node = hti->table->slots[index];
        // Get to the next entry.
        hti->slot += 1;
        if (node) { // If the entry is valid.
            return node;
        }
    }
    return (Node *) NULL;
}
