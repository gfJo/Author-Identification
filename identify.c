#include "text.h"
#include "pq.h"
#include "parser.h"
#include "ht.h"
#include "bf.h"
#include "node.h"
#include "speck.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>

#define OPTION     "d:n:k:l:emch"
#define MATCHES    5
#define NOISELIMIT 100
#define MAX_STRING 1024
#define WORD       "[a-zA-Z'-]+"

int main(int argc, char **argv) {

    // Initialize the variables.
    char *database_path = "lib.db";
    char *noise_path = "noise.txt";
    int matches = MATCHES;
    uint32_t noiselimit = NOISELIMIT;
    Metric metric = EUCLIDEAN;

    bool test_h = false;

    int opt = 0;
    while ((opt = getopt(argc, argv, OPTION)) != -1) {
        switch (opt) {
        case 'd': database_path = optarg; break;

        case 'n': noise_path = optarg; break;

        case 'k':
            // Num of matches.
            matches = atoi(optarg);
            break;

        case 'l': noiselimit = atoi(optarg); break;

        case 'e': metric = EUCLIDEAN; break;

        case 'm': metric = MANHATTAN; break;

        case 'c': metric = COSINE; break;

        case 'h': test_h = true; break;

        default: test_h = true; break;
        }
    }

    if (test_h) {
        printf("SYNOPSIS\n"
               "   Identifies the author of some anonymous text passed through stdin.\n"
               "   Basically computes k-nearest neighbors, where k = 1 by default.\n"
               "   The number of noise words that are removed is tunable.\n"
               "   Uses either Euclidean, Manhattan, or cosine distance as metric.\n"
               "\n"
               "USAGE\n"
               "   ./identify [OPTIONS]\n"
               "\n"
               "OPTIONS\n"
               "   -d database     Database of authors and texts [default: lib.db]\n"
               "   -k matches      Set number of top matches to display [default: 5]\n"
               "   -n noisefile    Set file of words to ignore [default: noise.txt]\n"
               "   -l limit        Set noise word limit [default: 100]\n"
               "   -e              Set distance metric as Euclidean distance [default]\n"
               "   -m              Set distance metric as Manhattan distance\n"
               "   -c              Set distance metric as cosine distance\n"
               "   -v              Enable verbose printing of program run\n"
               "   -h              Display program help and usage\n");
        exit(0);
    }

    // Create noise text.
    FILE *noise_file = fopen(noise_path, "r");
    Text *noise = text_create(noise_file, (Text *) NULL);

    // Create anonymous text.
    Text *anonymous = text_create(stdin, noise);
    // Open database and get the number n (capacity of the pq) to create pq.
    FILE *database = fopen(database_path, "r");

    char capa_buffer[MAX_STRING];

    if (!fgets(capa_buffer, MAX_STRING, database)) {
        fprintf(stderr, "Fail to read number of author/text pairs.\n");
        return -1;
    }
    uint32_t capacity = atoi(capa_buffer);
    PriorityQueue *q = pq_create(capacity);

    // Read from each pairs.
    char known_author[MAX_STRING];
    char known_path[MAX_STRING];
    // Read author name.
    while ((fgets(known_author, MAX_STRING, database)) != NULL) {
        // Read path.
        if ((fgets(known_path, MAX_STRING, database))) {
            int i = strlen(known_author);
            if (known_author[i - 1] == '\n') {
                known_author[i - 1] = '\0';
            }
            i = strlen(known_path);
            if (known_path[i - 1] == '\n') {
                known_path[i - 1] = '\0';
            }

            // Create a new text for each known author.
            FILE *author_file = fopen(known_path, "r");
            if (author_file) {
                Text *author = text_create(author_file, noise);
                double dist = text_dist(anonymous, author, metric);
                enqueue(q, known_author, dist);
                text_delete(&author);
                fclose(author_file);
            }
        }
    }

    // Dequeue from priority queue and print out the result.
    char *name = NULL;
    double distance;
    fprintf(
        stdout, "Top %u, metric: %s, noise limit: %u\n", matches, metric_names[metric], noiselimit);
    for (int k = 0; k < matches; k += 1) {
        if (!dequeue(q, &name, &distance)) {
            return 0;
        }
        fprintf(stdout, "%u) %s [%.15f]\n", k + 1, name, distance);
    }

    // Free.
    text_delete(&noise);
    text_delete(&anonymous);
    pq_delete(&q);

    // Close files.
    fclose(noise_file);
    fclose(database);

    return 0;
}
