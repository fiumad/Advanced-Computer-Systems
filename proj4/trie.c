#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "helper.h"
#include "trie.h"


int main() {
    const char *filename = "encoded_data_full.txt";

    // Load data into Trie
    TrieNode *trie_root = load_trie_from_file(filename);
    if (!trie_root) {
        fprintf(stderr, "Failed to load Trie from file\n");
        return EXIT_FAILURE;
    }

    printf("Trie successfully loaded from file.\n");

    // Perform prefix search
    char **results = malloc(INITIAL_CAPACITY * sizeof(char *));
    int **result_indices = malloc(INITIAL_CAPACITY * sizeof(int *));
    int *num_indices = malloc(INITIAL_CAPACITY * sizeof(int));
    int result_count = 0;
    int capacity = INITIAL_CAPACITY;

    // Use the Trie for queries or further processing...

    search_by_prefix(trie_root, "dan", &results, &result_indices, num_indices, &result_count);

    // Print results
    printf("Words matching prefix:\n");
    for (int i = 0; i < result_count; i++) {
        printf("%s: ", results[i]);
        printf("%s has %d indices\n", results[i], num_indices[i]);
        for (int j = 0; j < num_indices[i]; j++) {
            printf("%d ", result_indices[i][j]);
        }
        printf("\n");
        free(results[i]);
        free(result_indices[i]);
    }

    int *search_results;
    const char *word = "dano";
    search_results = search_trie(trie_root, word, &result_count);

    printf("key %s has %d occurrances\n", word, result_count);

    free(results);
    free(result_indices);
    free(num_indices);


    // Free the Trie when done
    free_trie(trie_root);

    return 0;
}

