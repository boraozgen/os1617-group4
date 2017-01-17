#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>    // time()

#define MAX_MATRIX_SIZE 10

/* Compute the maximum value in column [col] of matrix [inMatrix] */
int col_max(int** inMatrix, int col, int num_rows) {
	int i;
	int max = INT_MIN;

	for (i = 0; i < num_rows; i++)
	{
		if (inMatrix[i][col] > max)
		{
			max = inMatrix[i][col];
		}
	}

	return max;
}

/* Dynamically allocate matrix */
void alloc_matrix(int*** matrix_ptr, int num_rows, int num_cols) {
	*matrix_ptr = (int **)malloc(num_rows * sizeof(int*));
	for (int i = 0; i < num_rows; i++) (*matrix_ptr)[i] = (int *)malloc(num_cols * sizeof(int));
}

/* Fill matrix with random numbers */
void fill_matrix(int** matrix, int num_rows, int num_cols) {
    int i, j;

    for (i = 0; i < num_rows; i++)
        for (j = 0; j < num_cols; j++)
            matrix[i][j] = rand() % 100 - 50;
}

/* Print matrix to screen */
void print_matrix(int** matrix, int num_rows, int num_cols) {
	int i, j;
	for (i = 0; i < num_rows; i++)
	{
	    for (j = 0; j < num_cols; j++)
	         printf("%5d", matrix[i][j]);
	    printf("\n");
	}
}

int main(int argc, char *argv[]) {
	// Input matrices A, B
	int** matrixA = NULL; 
	int** matrixB = NULL; 
	// Sum matrix S
	int** matrixS = NULL;
	// Output matrix C
	int** matrixC = NULL;
	int num_rows, num_cols;
	int i, j;

	// TEST
	num_rows = 5;
	num_cols = 5;

	// Allocate memory for matrices
	alloc_matrix(&matrixA, num_rows, num_cols);
	alloc_matrix(&matrixB, num_rows, num_cols);
	alloc_matrix(&matrixS, num_rows, num_cols);
	alloc_matrix(&matrixC, 1, num_cols);

	// Randomly fill matrix A and B
	srand(time(NULL));
	fill_matrix(matrixA, num_rows, num_cols);
	fill_matrix(matrixB, num_rows, num_cols);

	printf("Matrix A:\n");
	print_matrix(matrixA, num_rows, num_cols);

	printf("Matrix B:\n");
	print_matrix(matrixB, num_rows, num_cols);

	// Compute the sum matrix S
	for (i = 0; i < num_rows; i++)
	{
		for (j = 0; j < num_cols; j++)
		{
			matrixS[i][j] = matrixA[i][j] + matrixB[i][j];
		}
	}

	printf("Sum matrix:\n");
	print_matrix(matrixS, num_rows, num_cols);

	// Compute max for each column. To be assigned to threads.
	for (i = 0; i < num_cols; i++)
	{
		matrixC[0][i] = col_max(matrixS, i, num_rows);
	}

	printf("Matrix C:\n");
	print_matrix(matrixC, 1, num_cols);

	exit(EXIT_SUCCESS); // exit normally
}