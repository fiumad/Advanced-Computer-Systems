#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ARRAY_SIZE 10000000  // Size of the array (adjust as needed)

int array[ARRAY_SIZE];  // The array that will be accessed

// Function for cache-friendly (sequential) access
void cache_friendly_access(int *arr, int size) {
    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];  // Sequential access
    }
    printf("Cache-friendly sum: %d\n", sum);
}

// Function for cache-hostile (random) access
void cache_hostile_access(int *arr, int size, int *random_indices) {
    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += arr[random_indices[i]];  // Random access
    }
    printf("Cache-hostile sum: %d\n", sum);
}

// Generate an array of random indices
void generate_random_indices(int *random_indices, int size) {
    for (int i = 0; i < size; i++) {
        random_indices[i] = rand() % size;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <friendly|hostile>\n", argv[0]);
        return 1;
    }

    // Seed the random number generator
    srand(time(NULL));

    // Fill the array with some data
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = i;
    }

    // Generate random indices for cache-hostile access
    int *random_indices = malloc(ARRAY_SIZE * sizeof(int));
    generate_random_indices(random_indices, ARRAY_SIZE);

    // Start measuring time
    clock_t start = clock();

    // Check the argument and perform the respective access pattern
    if (strcmp(argv[1], "friendly") == 0) {
        cache_friendly_access(array, ARRAY_SIZE);
    } else if (strcmp(argv[1], "hostile") == 0) {
        cache_hostile_access(array, ARRAY_SIZE, random_indices);
    } else {
        printf("Invalid argument. Use 'friendly' or 'hostile'.\n");
        free(random_indices);
        return 1;
    }

    // End measuring time and print the elapsed time
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Execution time: %f seconds\n", time_spent);

    // Free the allocated memory
    free(random_indices);

    return 0;
}

