#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>

struct matrix {
    int **matrix;
    int rows;
    int cols;
};

int** generate_matrix(int rows, int cols, float sparsity, bool print) {
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
    if (print){
      printf("Generated matrix (%d x %d) with sparsity %.2f:\n", rows, cols, sparsity);
      for (int i = 0; i < rows; i++) {
          for (int j = 0; j < cols; j++) {
              printf("%d ", matrix[i][j]);
          }
          printf("\n");
      }
    }
    return matrix;
}

int** multiply_matrices_no_opt(int **a, int **b) {
  if ()
  for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
          for (int k = 0; k < cols; k++) {
              c[i][j] += a[i][k] * b[k][j];
          }
      }
  }
}

void free_matrix(int **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
    return;
}

int main() {
    int rows, cols;
    float sparsity;
    int print_input;
    bool print;

    // Input for matrix dimensions and sparsity
    printf("Enter the number of rows: ");
    scanf("%d", &rows);
    printf("Enter the number of columns: ");
    scanf("%d", &cols);
    printf("Enter the sparsity (0 to 1, where 0 is dense and 1 is all zeros): ");
    scanf("%f", &sparsity);
    printf("Would you like to print matrices? (0 is no, 1 is yes)\n");
    scanf("%d", &print_input);
    if (print_input != 0 && print_input != 1){
      printf("Invalid input. Please enter 0 or 1.\n");
      return 1;
    }
    if(print == 1){
      print = true;
    } else {
      print = false;
    }

  // Generate matrix based on input
  int **matrix1 = generate_matrix(rows, cols, sparsity, true);
  int **matrix2 = generate_matrix(rows, cols, sparsity, true);
  free_matrix(matrix1, rows);

    return 0;
}

