#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// Define the matrix struct
struct matrix {
    int **data;  // Pointer to the matrix
    int rows;    // Number of rows
    int cols;    // Number of columns
};

// Function to generate a matrix dynamically and return it as a matrix struct
struct matrix generate_matrix(int rows, int cols, float sparsity, bool print) {
    struct matrix mat;
    mat.rows = rows;
    mat.cols = cols;
    
    // Dynamically allocate memory for the matrix
    mat.data = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        mat.data[i] = (int *)malloc(cols * sizeof(int));
    }

    // Fill matrix with values based on sparsity
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            float random_val = (float)rand() / RAND_MAX;
            if (random_val < sparsity) {
                mat.data[i][j] = 0; // Set to 0 based on sparsity
            } else {
                mat.data[i][j] = rand() % 100 + 1; // Random value between 1 and 100
            }
        }
    }

    // Print the matrix if requested
    if (print) {
        printf("Generated matrix (%d x %d) with sparsity %.2f:\n", rows, cols, sparsity);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                printf("%d ", mat.data[i][j]);
            }
            printf("\n");
        }
    }

    return mat;
}

// Function to multiply two matrices without optimizations
struct matrix multiply_matrices_no_opt(struct matrix a, struct matrix b) {
    // Check if the multiplication is valid
    if (a.cols != b.rows) {
        printf("Error: Incompatible matrix dimensions for multiplication.\n");
        exit(1);  // Exit with error if dimensions don't match
    }

    // Result matrix (a.rows x b.cols)
    struct matrix result;
    result.rows = a.rows;
    result.cols = b.cols;
    result.data = (int **)malloc(result.rows * sizeof(int *));
    for (int i = 0; i < result.rows; i++) {
        result.data[i] = (int *)malloc(result.cols * sizeof(int));
    }

    // Multiply matrices
    for (int i = 0; i < result.rows; i++) {
        for (int j = 0; j < result.cols; j++) {
            result.data[i][j] = 0;  // Initialize result matrix element
            for (int k = 0; k < a.cols; k++) {
                result.data[i][j] += a.data[i][k] * b.data[k][j];
            }
        }
    }

    return result;
}

// Function to free the memory of a matrix struct
void free_matrix(struct matrix mat) {
    for (int i = 0; i < mat.rows; i++) {
        free(mat.data[i]);
    }
    free(mat.data);
}

// Main function
int main() {
    int rows, cols;
    float sparsity;
    int print_input;
    bool print;

    srand(time(NULL));  // Seed random number generator

    // Input for matrix dimensions and sparsity
    printf("Enter the number of rows: ");
    scanf("%d", &rows);
    printf("Enter the number of columns: ");
    scanf("%d", &cols);
    printf("Enter the sparsity (0 to 1, where 0 is dense and 1 is all zeros): ");
    scanf("%f", &sparsity);
    printf("Would you like to print matrices? (0 is no, 1 is yes)\n");
    scanf("%d", &print_input);

    if (print_input == 1) {
        print = true;
    } else {
        print = false;
    }

    // Generate two matrices based on input
    struct matrix matrix1 = generate_matrix(rows, cols, sparsity, print);
    struct matrix matrix2 = generate_matrix(cols, rows, sparsity, print);  // Note: cols of matrix1 must equal rows of matrix2

    // Multiply the matrices
    struct matrix result_matrix = multiply_matrices_no_opt(matrix1, matrix2);

    // Print the result matrix
    if (print) {
        printf("Result of matrix multiplication:\n");
        for (int i = 0; i < result_matrix.rows; i++) {
            for (int j = 0; j < result_matrix.cols; j++) {
                printf("%d ", result_matrix.data[i][j]);
            }
            printf("\n");
        }
    }

    // Free all matrices
    free_matrix(matrix1);
    free_matrix(matrix2);
    free_matrix(result_matrix);

    return 0;
}

