#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GIGABYTE (1024 * 1024 * 1024)

int main(int argc, char *argv[]) { if (argc != 3) {
        fprintf(stderr, "Usage: %s <element_size_in_bytes>, %s <read_ratio x 100>\n", argv[0], argv[1]);
        return 1;
    }

    // Get element size from the command-line argument
    size_t element_size = (size_t)atoi(argv[1]);
    if (element_size == 0) {
        fprintf(stderr, "Invalid element size. Must be a positive integer.\n");
        return 1;
    }

    // Get read ratio from the command-line argument
    size_t read_ratio = (size_t)atoi(argv[2]);
    if (read_ratio <= 0 || read_ratio > 100){
      fprintf(stderr, "Invalid read ratio. Must be an integer between 0 and 100.\n");
      return 1;
    }

    // Calculate the number of elements that fit in 1GB
    size_t num_elements = GIGABYTE / element_size;

    // Allocate 1GB of memory
    void *memory = malloc(GIGABYTE);
    if (memory == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    size_t read_elements = num_elements * (read_ratio / 100);

    // Writing to memory
    for (size_t i = 0; i < num_elements; i++) {
        memset((char *)memory + i * element_size, (char)(i % 256), element_size);
    }
    printf("Memory write complete (element size: %zu bytes).\n", element_size);

    // Reading from memory
    size_t error_count = 0;
    for (size_t i = 0; i < read_elements; i++) {
        char *element = (char *)memory + i * element_size;
        if (*element != (char)(i % 256)) {
            fprintf(stderr, "Memory error at element %zu\n", i);
            error_count++;
            if (error_count > 10) {
                break;  // Stop after finding 10 errors
            }
        }
    }
    printf("Memory read complete.\n");

    // Free the allocated memory
    free(memory);
    printf("Memory freed.\n");

    return 0;
}

