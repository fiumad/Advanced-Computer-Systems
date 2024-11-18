#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "helper.h"
#include "trie.h"


int main(int argc, char *argv[]) {
    /*
    const char *filename = "encoded_data_full.txt";

    // Load data into Trie
    TrieNode *trie_root = load_trie_from_file(filename);
    if (!trie_root) {
        fprintf(stderr, "Failed to load Trie from file\n");
        return EXIT_FAILURE;
    }
    printf("Trie successfully loaded from file.\n");
    */

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <num_threads>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Get the filename and number of threads from command-line arguments
    const char *filename = argv[1];
    int num_threads = atoi(argv[2]);

    if (num_threads <= 0) {
        fprintf(stderr, "Invalid number of threads. Exiting.\n");
        exit(EXIT_FAILURE);
    }
    TrieNode *trie_root = create_trie_node();

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // Get the number of lines in the file
    long total_lines = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') total_lines++;
    }
    fclose(file);

    pthread_t threads[num_threads];
    ThreadArgs thread_args[num_threads];

    long lines_per_thread = total_lines / num_threads;

    // Create threads to process chunks of the file
    for (int i = 0; i < num_threads; i++) {
        thread_args[i].root = trie_root;
        thread_args[i].filename = filename;
        thread_args[i].start_line = i * lines_per_thread;
        thread_args[i].end_line = (i == num_threads - 1) ? total_lines : (i + 1) * lines_per_thread;

        pthread_create(&threads[i], NULL, ingest_column_data_thread, &thread_args[i]);
    }

    // Join all threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

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

