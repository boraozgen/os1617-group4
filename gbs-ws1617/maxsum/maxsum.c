#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>    // time()
#include <pthread.h>
#include <unistd.h>

#define MAX_MATRIX_SIZE 10

/* Thread arguments struct */
typedef struct thread_args_t {
	int** inMatrix;
	int** outMatrix;
	int col;
	int num_rows;
} ThreadArgs;

/* Compute the maximum value in column [col] of matrix [inMatrix] */
void col_max(int** inMatrix, int** outMatrix, int col, int num_rows) {
	int i;
	int max = INT_MIN;

	for (i = 0; i < num_rows; i++)
	{
		if (inMatrix[i][col] > max)
		{
			max = inMatrix[i][col];
		}
	}

	outMatrix[0][col] = max;
}

/* Thread routine */
void* thread_routine(void* thread_args) {
	ThreadArgs *data;
	data = (ThreadArgs*) thread_args;
	col_max(data->inMatrix, data->outMatrix, data->col, data->num_rows);
	pthread_exit(NULL);
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
	int i, j, rc;

	// TEST
	num_rows = 5;
	num_cols = 5;

	pthread_t threads[num_cols];
	pthread_attr_t attr;
	void *status;

	/* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

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
	ThreadArgs args[num_cols];
	for (i = 0; i < num_cols; i++)
	{
		args[i].inMatrix = matrixS;
		args[i].outMatrix = matrixC;
		args[i].col = i;
		args[i].num_rows = num_rows;
		rc = pthread_create(&threads[i], &attr, thread_routine, (void*) &args[i]);
		if (rc){
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
       	}
	}

	/* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
    for (i = 0; i < num_cols; i++) {
		rc = pthread_join(threads[i], &status);
		if (rc) {
			printf("ERROR; return code from pthread_join() is %d\n", rc);
			exit(-1);
		}
   	}

	printf("Matrix C:\n");
	print_matrix(matrixC, 1, num_cols);

	pthread_exit(NULL);
	exit(EXIT_SUCCESS); // exit normally
}

/* Helpful link: https://computing.llnl.gov/tutorials/pthreads/ */