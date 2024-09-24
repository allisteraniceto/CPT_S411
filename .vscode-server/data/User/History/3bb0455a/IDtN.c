#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define X 100
#define Y 100
#define Z 100

//put values into arrays
void initialize_arrays(int ***A, int ***B) {
    for (int i = 0; i < X; i++) {
        for (int j = 0; j < Y; j++) {
            for (int k = 0; k < Z; k++) {
                A[i][j][k] = 1; 
                B[i][j][k] = 1; 
            }
        }
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // allocate memory for the 3D arrays
    int ***A = (int ***)malloc(X * sizeof(int **));
    int ***B = (int ***)malloc(X * sizeof(int **));
    int ***C = (int ***)malloc(X * sizeof(int **));
    for (int i = 0; i < X; i++) {
        A[i] = (int **)malloc(Y * sizeof(int *));
        B[i] = (int **)malloc(Y * sizeof(int *));
        C[i] = (int **)malloc(Y * sizeof(int *));
        for (int j = 0; j < Y; j++) {
            A[i][j] = (int *)malloc(Z * sizeof(int));
            B[i][j] = (int *)malloc(Z * sizeof(int));
            C[i][j] = (int *)malloc(Z * sizeof(int));
        }
    }

    double startwtime, endwtime;

    if (world_rank == 0) {
        initialize_arrays(A, B);
    }

    // broadcast A and B to all processes
    MPI_Bcast(&(A[0][0][0]), X*Y*Z, MPI_INT, 0, MPI_COMM_WORLD); 
    MPI_Bcast(&(B[0][0][0]), X*Y*Z, MPI_INT, 0, MPI_COMM_WORLD);

    int chunk_size = X / world_size;
    int start = world_rank * chunk_size; // partition work for each process
    int end = (world_rank + 1) * chunk_size; // +1 to include the last element

    startwtime = MPI_Wtime();

    for (int i = start; i < end; i++) {
        for (int j = 0; j < Y; j++) {
            for (int k = 0; k < Z; k++) {
                C[i][j][k] = A[i][j][k] + B[i][j][k];
            }
        }
    }

    // Gather the results to the root process
    MPI_Gather(&(C[start][0][0]), chunk_size*Y*Z, MPI_INT, 
               &(C[0][0][0]), chunk_size*Y*Z, MPI_INT, 
               0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        endwtime = MPI_Wtime();
        printf("n = %d\n", world_size);
        printf("Wall clock time = %f\n", endwtime - startwtime);
    }

    // Free the allocated memory
    for (int i = 0; i < X; i++) {
        for (int j = 0; j < Y; j++) {
            free(A[i][j]);
            free(B[i][j]);
            free(C[i][j]);
        }
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);

    MPI_Finalize();
    return 0;
}

/*
n = 1,2,4,8,16,32

OUTPUTS:
n=1
Wall clock time = 0.013496

*/