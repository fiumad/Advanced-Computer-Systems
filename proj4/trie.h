#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 1024
#define ALPHABET_SIZE 26


typedef struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE]; // Assuming lowercase a-z
    int *indices;                 // List of indices for this word
    int index_count;              // Number of indices
    int is_end_of_word;           // Marks end of a word
} TrieNode;

void free_trie(TrieNode *root) {
    if (!root) return;

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        free_trie(root->children[i]);
    }

    free(root->indices);
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
        printf("index count is %d\n", num_indices[*result_count]);
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
        printf("searching for %c\n", word[i]);
        int char_index = word[i] - 'a'; // Assumes lowercase a-z
        if (char_index < 0 || char_index >= 26) {
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
        *index_count = current->index_count; //PROBLEM: index_count is not being updated
        printf("found word %s\n", word);
        printf("word has %d indices\n", current->index_count);
        return current->indices; // Return the indices associated with the word
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
    return node;
}

void trie_insert(TrieNode *root, const char *key, int index, int counter) {
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

    // Append the index to the list of indices
    node->indices = realloc(node->indices, (node->index_count + 1) * sizeof(int));
    node->indices[node->index_count++] = index;
    node->index_count = counter;
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
        printf("counter is %d\n", counter);
        if (counter <= 0) {
            fprintf(stderr, "Invalid counter value %d at line %d\n", counter, line_number);
            continue;
        }

        int *indices = malloc(counter * sizeof(int));
        for (int i = 0; i < counter; i++) {
            printf("index %d is %d\n", i, indices[i]);
        }
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
        }

        // Insert reconstructed data into the Trie
        for (int i = 0; i < counter; i++) {
            trie_insert(trie_root, key, indices[i], counter);
        }

        free(indices);
    }

    fclose(file);
    return trie_root;
}
