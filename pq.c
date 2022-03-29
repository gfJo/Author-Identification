#include "pq.h"

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

typedef struct PQEntry PQEntry;

struct PQEntry { // From Eugene's section.
    char *author;
    double dist;
};

struct PriorityQueue {
    uint32_t size;
    uint32_t capacity;
    PQEntry **E;
};

// Functions for entry are below functions for pq.
PQEntry *entry_create(char *author, double dist);
void entry_delete(PQEntry **E);
void entry_print(PQEntry *E);

// Functions for pq.
PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *q = (PriorityQueue *) malloc(sizeof(PriorityQueue)); // Create a queue.
    if (q) { // If succeed to create a queue.
        // Initialize size and capacity.
        q->size = 0;
        q->capacity = capacity;
        q->E = (PQEntry **) calloc(capacity, sizeof(PQEntry *)); // Create pointers for entry.
        if (q->E) {
            // Initialize all the entry pointers.
            for (uint32_t i = 0; i < capacity; i += 1) {
                q->E[i] = NULL;
            }
            return q;
        } else {
            free(q);
            q = NULL;
        }
    }
    return (PriorityQueue *) NULL;
}

void pq_delete(PriorityQueue **q) {
    if (*q) {
        if ((*q)->E) {

            // Delete the entry remained in the queue.
            if (!pq_empty(*q)) {
                for (uint32_t i = 0; i < (*q)->size; i += 1) {
                    if ((*q)->E[i]) {
                        entry_delete(&((*q)->E[i]));
                    }
                }
            }

            // Then free the memory for text.
            free((*q)->E);
        }
        // Free the pq and set the pointer to NULL.
        (*q)->size = 0;
        free(*q);
        *q = NULL;
    }
}

bool pq_empty(PriorityQueue *q) {
    return (q->size == 0);
}

bool pq_full(PriorityQueue *q) {
    return (q->size == q->capacity);
}

uint32_t pq_size(PriorityQueue *q) {
    return q->size;
}

bool enqueue(PriorityQueue *q, char *author, double dist) {
    // Using insertion sort.

    if (q) {
        // If queue still has space for the new elements.
        if (!pq_full(q)) {

            uint32_t index = q->size;
            PQEntry *e = entry_create(author, dist); // Create an entry.

            while (index > 0) {
                // Good place for new entry.
                if (e->dist > q->E[index - 1]->dist) {
                    q->E[index] = e;
                    q->size += 1;
                    return true;
                } else { // Bad place, check next place.
                    q->E[index] = q->E[index - 1];
                    index -= 1;
                }
            }
            // Reach the end of the queue.
            q->E[index] = e;
            q->size += 1;
            return true;
        }
        return false;
    }
    printf("Fail to create pq!\n");
    return false;
}

bool dequeue(PriorityQueue *q, char **author, double *dist) {
    if (q) {
        // If there is items for dequeue.
        if (!pq_empty(q)) {

            // Output the author and dist.
            *author = q->E[0]->author;
            *dist = q->E[0]->dist;

            entry_delete(&(q->E[0])); // Free the dequeued entry.
            q->size -= 1;

            // If there is remained entry in the pq, shift forward all of them.
            if (!pq_empty(q)) {
                for (uint32_t i = 1; i <= q->size; i += 1) {
                    q->E[i - 1] = q->E[i];
                }
            }
            return true;
        }
    }
    return false;
}

void pq_print(PriorityQueue *q) {
    for (uint32_t i = 0; i < q->size; i += 1) {
        entry_print(q->E[i]);
    }
}

// Below are the functions for Priority Queue Entry.

// Create Entry which includes pointer of author and distance.
PQEntry *entry_create(char *author, double dist) {
    PQEntry *e = (PQEntry *) malloc(sizeof(PQEntry));
    if (e) {
        e->author = strdup(author);
        e->dist = dist;
        return e;
    }
    return (PQEntry *) NULL;
}

void entry_delete(PQEntry **e) {
    if (*e) {
        free(*e);
        *e = NULL;
    }
}

void entry_print(PQEntry *e) {
    printf("Author Name: %s, Distance: %f\n", e->author, e->dist);
}
