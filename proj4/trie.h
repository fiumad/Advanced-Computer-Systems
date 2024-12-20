#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define INITIAL_CAPACITY 1024
#define ALPHABET_SIZE 26
#define MAX_LINE_LENGTH 256

// Mutex for thread safety during trie insertion
pthread_mutex_t trie_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE]; // Assuming lowercase a-z
    int *indices;                 // List of indices for this word
    int index_count;              // Number of indices
    int is_end_of_word;           // Marks end of a word
    pthread_mutex_t lock;
} TrieNode;


typedef struct {
    TrieNode *root;
    char *filename;
    long start_line;
    long end_line;
} ThreadArgs;

void free_trie(TrieNode *root) {
    if (!root) return;

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        free_trie(root->children[i]);
    }

    free(root->indices);
    pthread_mutex_destroy(&root->lock);
    free(root);
}

// Function to grow a dynamic array
void grow_arrays(char ***results, int ***result_indices, int *num_indices, int *capacity) {
    *capacity *= 2;
    *results = realloc(*results, (*capacity) * sizeof(char *));
    *result_indices = realloc(*result_indices, (*capacity) * sizeof(int *));
    num_indices = realloc(num_indices, (*capacity) * sizeof(int));
    if (!*results || !*result_indices || !num_indices) {
        perror("Failed to resize arrays");
        exit(EXIT_FAILURE);
    }
}

void collect_words(TrieNode *node, char *prefix, char ***results, int ***result_indices, int *result_count, int *num_indices, int *capacity) {
    if (!node) return;

    // If this node represents the end of a word, add it to the results
    if (node->is_end_of_word) {
        // Resize arrays if necessary
        if (*result_count >= *capacity - 1) {
            grow_arrays(results, result_indices, num_indices, capacity);
        }

        (*results)[*result_count] = strdup(prefix); // Copy the prefix as a found word
        (*result_indices)[*result_count] = malloc(node->index_count * sizeof(int));
        memcpy((*result_indices)[*result_count], node->indices, node->index_count * sizeof(int));
        num_indices[*result_count] = node->index_count;
        //printf("index count is %d\n", num_indices[*result_count]);
        (*result_count)++;
    }

    // Traverse children nodes
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) {
            // Allocate memory for the extended prefix
            size_t new_length = strlen(prefix) + 2; // One for the new character, one for null terminator
            if (prefix[strlen(prefix)] != '\0') {
              printf("\n prefix is not null terminated\n");
              return;
            }
            char *next_prefix = malloc(new_length);
            snprintf(next_prefix, new_length, "%s%c", prefix, 'a' + i); // Append character to prefix
            collect_words(node->children[i], next_prefix, results, result_indices, result_count, num_indices, capacity);
            free(next_prefix); // Free the dynamically allocated prefix
        }
    }
}

void search_by_prefix(TrieNode *root, const char *prefix, char ***results, int ***result_indices, int *num_indices, int *result_count) {
  TrieNode *node = root;
  for (int i = 0; prefix[i]; i++) {
      int index = prefix[i] - 'a'; // Assumes lowercase alphabet
      if (!node->children[index]) {
          printf("No matches found for prefix '%s'\n", prefix);
          *result_count = 0;
          return;
      }
      node = node->children[index];
  }

    // Collect words from the prefix node
    int capacity = INITIAL_CAPACITY;
    char* pref = strdup(prefix);
    collect_words(node, pref, results, result_indices, result_count, num_indices, &capacity);
    free(pref);
}

// Search function
int *search_trie(TrieNode *root, const char *word, int *index_count) {
    TrieNode *current = root;

    for (int i = 0; word[i] != '\0'; i++) {
        //printf("searching for %c\n", word[i]);
        int char_index = word[i] - 'a'; // Assumes lowercase a-z
        if (char_index < 0 || char_index >= ALPHABET_SIZE) {
            fprintf(stderr, "Invalid character in word: '%c'\n", word[i]);
            return NULL; // Invalid character for the Trie
        }

        if (!current->children[char_index]) {
            return NULL; // Word not found
        }

        current = current->children[char_index];
    }

    // Check if the final node is the end of a word
    if (current && current->is_end_of_word) {
        *index_count = current->index_count; // Update the index count
        //printf("found word %s\n", word);
        //printf("word has %d indices\n", current->index_count);
        return current->indices; // Return the pointer to the list of indices
    }

    return NULL; // Word not found
}
// Function to create a new Trie node
TrieNode *create_trie_node() {
    TrieNode *node = malloc(sizeof(TrieNode));
    if (!node) {
        perror("Failed to allocate TrieNode");
        exit(EXIT_FAILURE);
    }
    node->is_end_of_word = 0;
    node->indices = NULL;
    node->index_count = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        node->children[i] = NULL;
    }

    // Initialize the mutex
    if (pthread_mutex_init(&node->lock, NULL) != 0) {
        perror("Failed to initialize mutex");
        free(node);
        exit(EXIT_FAILURE);
    }

    return node;
}

void trie_insert(TrieNode *root, const char *key, int *new_indices, int new_count) {
    TrieNode *node = root;
    for (int i = 0; key[i] != '\0'; i++) {
        int char_index = key[i] - 'a'; // Convert char to index
        if (char_index < 0 || char_index >= ALPHABET_SIZE) {
            fprintf(stderr, "Unsupported character: %c\n", key[i]);
            return; // Ignore invalid characters
        }
        if (!node->children[char_index]) {
            node->children[char_index] = create_trie_node();
        }
        node = node->children[char_index];
    }
    node->is_end_of_word = 1;

    // Append new indices to the existing indices array
    node->indices = realloc(node->indices, (node->index_count + new_count) * sizeof(int));
    if (!node->indices) {
        perror("Memory allocation failed in trie_insert");
        exit(EXIT_FAILURE);
    }

    memcpy(&node->indices[node->index_count], new_indices, new_count * sizeof(int));
    node->index_count += new_count;
}

// Thread-safe insertion into the trie
void threaded_trie_insert(TrieNode *root, const char *key, int *new_indices, int new_count) {
    TrieNode *node = root;

    // Lock the root node before starting
    pthread_mutex_lock(&node->lock);

    for (int i = 0; key[i] != '\0'; i++) {
        int char_index = key[i] - 'a'; // Convert char to index

        if (char_index < 0 || char_index >= ALPHABET_SIZE) {
            fprintf(stderr, "Unsupported character: %c\n", key[i]);
            pthread_mutex_unlock(&node->lock);
            return; // Ignore invalid characters
        }

        // Lock the current node's mutex
        if (!node->children[char_index]) {
            // Allocate a new child node under the lock
            node->children[char_index] = create_trie_node();
        }

        // Move to the child node, lock it, and unlock the parent
        TrieNode *next_node = node->children[char_index];
        pthread_mutex_lock(&next_node->lock);
        pthread_mutex_unlock(&node->lock);
        node = next_node;
    }

    // Update the node at the end of the key
    node->is_end_of_word = 1;

    // Append new indices to the existing indices array
    node->indices = realloc(node->indices, (node->index_count + new_count) * sizeof(int));
    if (!node->indices) {
        perror("Memory allocation failed in trie_insert");
        pthread_mutex_unlock(&node->lock);
        exit(EXIT_FAILURE);
    }

    memcpy(&node->indices[node->index_count], new_indices, new_count * sizeof(int));
    node->index_count += new_count;

    // Unlock the last node
    pthread_mutex_unlock(&node->lock);
}

// Function to ingest raw column data from a portion of the file into the trie
void *ingest_column_data_thread(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    FILE *file = fopen(args->filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    int line_index = 0;

    // Skip to the starting line
    for (long i = 0; i < args->start_line; i++) {
        if (!fgets(line, sizeof(line), file)) break;
    }

    // Process lines within the specified range
    while (line_index < args->end_line - args->start_line && fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';  // Remove the newline character

        int index = args->start_line + line_index++;
        threaded_trie_insert(args->root, line, &index, 1); // Insert the word with the index
    }

    fclose(file);
    return NULL;
}

// Function to ingest raw column data from a file into the trie
void ingest_column_data(const char *filename, TrieNode *root) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char line[256]; // Assuming the line length won't exceed 255 characters
    int line_index = 0;

    // Read the file line by line
    while (fgets(line, sizeof(line), file)) {
        // Remove the newline character from the end of the line
        line[strcspn(line, "\n")] = '\0';

        // Insert the line into the trie with the current line_index as the associated index
        int index = line_index++; // Use the current line as the index
        trie_insert(root, line, &index, 1); // Insert the word with the index

    }

    fclose(file);
}

TrieNode *load_trie_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file for reading");
        return NULL;
    }

    printf("Loading Trie from file...\n");

    TrieNode *trie_root = create_trie_node();
    char buffer[999999];
    int line_number = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        if (buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer) - 1] = '\0'; // Remove newline
        }

        line_number++;

        char *key = strtok(buffer, " ");
        if (!key) {
            fprintf(stderr, "Skipping malformed line %d due to key error\n", line_number);
            continue;
        }

        char *counter_str = strtok(NULL, " ");
        if (!counter_str) {
            fprintf(stderr, "Skipping malformed line %d due to counter error\n", line_number);
            continue;
        }

        int counter = atoi(counter_str);
        //printf("Counter is %d\n", counter);
        if (counter <= 0) {
            fprintf(stderr, "Invalid counter value %d at line %d\n", counter, line_number);
            continue;
        }

        int *indices = malloc(counter * sizeof(int));
        if (!indices) {
            perror("Memory allocation failed");
            fclose(file);
            free_trie(trie_root); // Free Trie if memory allocation fails
            return NULL;
        }

        for (int i = 0; i < counter; i++) {
            char *index_str = strtok(NULL, " ");
            if (!index_str) {
                fprintf(stderr, "Skipping malformed line %d due to missing index string\n", line_number);
                free(indices);
                fclose(file);
                free_trie(trie_root);
                return NULL;
            }
            indices[i] = atoi(index_str);
            //printf("Index %d is %d\n", i, indices[i]);
        }

        trie_insert(trie_root, key, indices, counter);

        free(indices);
    }

    fclose(file);
    return trie_root;
}
