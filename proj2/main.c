#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>

void generate_matrix(int rows, int cols, float sparsity) {
    // Dynamically allocate memory for a matrix of size rows x cols
    int **matrix = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (int *)malloc(cols * sizeof(int));
    }

    // Seed random number generator
    srand(time(NULL));

    // Fill matrix with values based on sparsity
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            // Generate a random number between 0 and 1 to decide if the element should be 0 or random
            float random_val = (float)rand() / RAND_MAX;
            if (random_val < sparsity) {
                matrix[i][j] = 0; // Set to 0 based on sparsity
            } else {
                matrix[i][j] = rand() % 100 + 1; // Random value between 1 and 100
            }
        }
    }

    // Print the matrix
    printf("Generated matrix (%d x %d) with sparsity %.2f:\n", rows, cols, sparsity);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }

    // Free allocated memory
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

int main() {
    int rows, cols;
    float sparsity;

    // Input for matrix dimensions and sparsity
    printf("Enter the number of rows: ");
    scanf("%d", &rows);
    printf("Enter the number of columns: ");
    scanf("%d", &cols);
    printf("Enter the sparsity (0 to 1, where 0 is dense and 1 is all zeros): ");
    scanf("%f", &sparsity);

    // Generate matrix based on input
    generate_matrix(rows, cols, sparsity);

    return 0;
}

