#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

typedef struct {
    int *line_numbers;  // Array to store line numbers where the key appears
    int count;          // Number of occurrences
} SearchResult;

// Function to search for a key in the column data
SearchResult search_key_in_column(const char *filename, const char *key) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    SearchResult result;
    result.line_numbers = NULL;
    result.count = 0;

    char line[MAX_LINE_LENGTH];
    int line_number = 0;

    while (fgets(line, sizeof(line), file)) {
        line_number++;

        // Remove trailing newline character, if present
        line[strcspn(line, "\n")] = '\0';

        // Check if the line matches the key
        if (strcmp(line, key) == 0) {
            result.line_numbers = realloc(result.line_numbers, (result.count + 1) * sizeof(int));
            if (!result.line_numbers) {
                perror("Memory allocation failed");
                fclose(file);
                exit(EXIT_FAILURE);
            }
            result.line_numbers[result.count++] = line_number;
        }
    }

    fclose(file);
    return result;
}

// Function to search for a prefix in the column data
SearchResult search_prefix_in_column(const char *filename, const char *prefix) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    SearchResult result;
    result.line_numbers = NULL;
    result.count = 0;

    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    size_t prefix_length = strlen(prefix);

    while (fgets(line, sizeof(line), file)) {
        line_number++;

        // Remove trailing newline character, if present
        line[strcspn(line, "\n")] = '\0';

        // Check if the line starts with the prefix
        if (strncmp(line, prefix, prefix_length) == 0) {
            result.line_numbers = realloc(result.line_numbers, (result.count + 1) * sizeof(int));
            if (!result.line_numbers) {
                perror("Memory allocation failed");
                fclose(file);
                exit(EXIT_FAILURE);
            }
            result.line_numbers[result.count++] = line_number;
        }
    }

    fclose(file);
    return result;
}

// Function to free the allocated resources in SearchResult
void free_search_result(SearchResult *result) {
    free(result->line_numbers);
    result->line_numbers = NULL;
    result->count = 0;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <filename> <mode> <key_or_prefix>\n", argv[0]);
        fprintf(stderr, "Modes: exact, prefix\n");
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];
    const char *mode = argv[2];
    const char *key_or_prefix = argv[3];

    SearchResult result;

    if (strcmp(mode, "exact") == 0) {
        result = search_key_in_column(filename, key_or_prefix);
    } else if (strcmp(mode, "prefix") == 0) {
        result = search_prefix_in_column(filename, key_or_prefix);
    } else {
        fprintf(stderr, "Invalid mode: %s\n", mode);
        return EXIT_FAILURE;
    }

    // Print the results
    if (result.count > 0) {
        printf("'%s' found %d time(s) at line(s):\n", key_or_prefix, result.count);
        for (int i = 0; i < result.count; i++) {
            printf("%d\n", result.line_numbers[i]);
        }
    } else {
        printf("'%s' not found in the file.\n", key_or_prefix);
    }

    // Free allocated resources
    free_search_result(&result);

    return EXIT_SUCCESS;
}
