#include "node.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

Node *node_create(char *word) {
    Node *n = (Node *) malloc(sizeof(Node)); // Create a node.
    if (n != NULL) {
        n->word = strdup(word);
        n->count = 1;
    }
    return n;
}

void node_delete(Node **n) {
    if (*n) {
        free(*n);
        *n = NULL;
    }
}

void node_print(Node *n) {
    if (n) {
        printf("Char: %s, Count: %" PRIu32 "\n", n->word, n->count);
    }
}
