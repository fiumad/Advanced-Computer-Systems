#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_BUCKETS 2000000

// Dynamic array to store indices
typedef struct IndexList {
    int *indices;
    int size;
    int capacity;
} IndexList;

// Linked list node structure
typedef struct Node {
    char *key;
    int counter;
    IndexList index_list; // List of indices where the key appears
    struct Node *next;
} Node;

// Bucket structure
typedef struct Bucket {
    Node *head;
} Bucket;

// Hash table structure
typedef struct HashTable {
    Bucket **buckets;
} HashTable;

// Hash function
unsigned int hash_function(const char *key) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash * 31) + *key++;
    }
    return hash % NUM_BUCKETS;
}

// Initialize an index list
void init_index_list(IndexList *list) {
    list->size = 0;
    list->capacity = 4; // Start with a small capacity
    list->indices = malloc(list->capacity * sizeof(int));
    if (!list->indices) {
        perror("Failed to allocate memory for index list");
        exit(EXIT_FAILURE);
    }
}

// Add an index to the index list
void add_index(IndexList *list, int index) {
    if (list->size == list->capacity) {
        list->capacity *= 2; // Double the capacity
        list->indices = realloc(list->indices, list->capacity * sizeof(int));
        if (!list->indices) {
            perror("Failed to reallocate memory for index list");
            exit(EXIT_FAILURE);
        }
    }
    list->indices[list->size++] = index;
}

// Free the index list
void free_index_list(IndexList *list) {
    free(list->indices);
}

// Create a new node
Node *create_node(const char *key, int index) {
    Node *new_node = malloc(sizeof(Node));
    if (!new_node) {
        perror("Failed to allocate memory for node");
        exit(EXIT_FAILURE);
    }
    new_node->key = strdup(key);
    if (!new_node->key) {
        perror("Failed to allocate memory for key");
        exit(EXIT_FAILURE);
    }
    new_node->counter = 1; // Initialize counter to 1
    init_index_list(&new_node->index_list);
    add_index(&new_node->index_list, index); // Add the first index
    new_node->next = NULL;
    return new_node;
}

// Create a new hash table
HashTable *create_hash_table() {
    HashTable *hash_table = malloc(sizeof(HashTable));
    if (!hash_table) {
        perror("Failed to allocate memory for hash table");
        exit(EXIT_FAILURE);
    }
    hash_table->buckets = calloc(NUM_BUCKETS, sizeof(Bucket *));
    if (!hash_table->buckets) {
        perror("Failed to allocate memory for buckets");
        exit(EXIT_FAILURE);
    }
    return hash_table;
}

// Insert a key into the hash table
void insert(HashTable *hash_table, const char *key, int index) {
    unsigned int hash_index = hash_function(key);
    Bucket *bucket = hash_table->buckets[hash_index];

    // If the bucket is empty, create a new one
    if (!bucket) {
        bucket = malloc(sizeof(Bucket));
        if (!bucket) {
            perror("Failed to allocate memory for bucket");
            exit(EXIT_FAILURE);
        }
        bucket->head = NULL;
        hash_table->buckets[hash_index] = bucket;
    }

    // Check if the key already exists in the linked list
    Node *current = bucket->head;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            current->counter++; // Increment counter if key matches
            add_index(&current->index_list, index); // Add index to list
            return;
        }
        current = current->next;
    }

    // Key not found, add a new node at the beginning of the linked list
    Node *new_node = create_node(key, index);
    new_node->next = bucket->head;
    bucket->head = new_node;
}

// Search for a key in the hash table and return its node
Node *search(HashTable *hash_table, const char *key) {
    unsigned int hash_index = hash_function(key);
    Bucket *bucket = hash_table->buckets[hash_index];

    if (!bucket) {
        return NULL; // Key not found
    }

    Node *current = bucket->head;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return current; // Return the node if key matches
        }
        current = current->next;
    }

    return NULL; // Key not found
}

// Free the hash table
void free_hash_table(HashTable *hash_table) {
    for (int i = 0; i < NUM_BUCKETS; i++) {
        Bucket *bucket = hash_table->buckets[i];
        if (bucket) {
            Node *current = bucket->head;
            while (current) {
                Node *temp = current;
                current = current->next;
                free(temp->key);
                free_index_list(&temp->index_list);
                free(temp);
            }
            free(bucket);
        }
    }
    free(hash_table->buckets);
    free(hash_table);
}

