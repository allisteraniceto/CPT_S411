#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define X 100
#define Y 100
#define Z 100

void initialize_arrays(int ***A, int ***B) {
    for (int i = 0; i < X; i++) {
        for (int j = 0; j < Y; j++) {
            for (int k = 0; k < Z; k++) {
                A[i][j][k] = 1; // Example initialization
                B[i][j][k] = 2; // Example initialization
            }
        }
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Allocate memory for the 3D arrays
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

    int chunk_size = X / world_size;
    int ***local_A = (int ***)malloc(chunk_size * sizeof(int **));
    int ***local_B = (int ***)malloc(chunk_size * sizeof(int **));
    int ***local_C = (int ***)malloc(chunk_size * sizeof(int **));
    for (int i = 0; i < chunk_size; i++) {
        local_A[i] = (int **)malloc(Y * sizeof(int *));
        local_B[i] = (int **)malloc(Y * sizeof(int *));
        local_C[i] = (int **)malloc(Y * sizeof(int *));
        for (int j = 0; j < Y; j++) {
            local_A[i][j] = (int *)malloc(Z * sizeof(int));
            local_B[i][j] = (int *)malloc(Z * sizeof(int));
            local_C[i][j] = (int *)malloc(Z * sizeof(int));
        }
    }

    // Scatter A and B to all processes
    MPI_Scatter(&(A[0][0][0]), chunk_size*Y*Z, MPI_INT, 
                &(local_A[0][0][0]), chunk_size*Y*Z, MPI_INT, 
                0, MPI_COMM_WORLD);
    MPI_Scatter(&(B[0][0][0]), chunk_size*Y*Z, MPI_INT, 
                &(local_B[0][0][0]), chunk_size*Y*Z, MPI_INT, 
                0, MPI_COMM_WORLD);

    startwtime = MPI_Wtime();

    for (int i = 0; i < chunk_size; i++) {
        for (int j = 0; j < Y; j++) {
            for (int k = 0; k < Z; k++) {
                local_C[i][j][k] = local_A[i][j][k] + local_B[i][j][k];
            }
        }
    }

    // Gather the results to the root process
    MPI_Gather(&(local_C[0][0][0]), chunk_size*Y*Z, MPI_INT, 
               &(C[0][0][0]), chunk_size*Y*Z, MPI_INT, 
               0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        endwtime = MPI_Wtime();
        printf("n=%d\n", world_size);
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

    for (int i = 0; i < chunk_size; i++) {
        for (int j = 0; j < Y; j++) {
            free(local_A[i][j]);
            free(local_B[i][j]);
            free(local_C[i][j]);
        }
        free(local_A[i]);
        free(local_B[i]);
        free(local_C[i]);
    }
    free(local_A);
    free(local_B);
    free(local_C);

    MPI_Finalize();
    return 0;
}
/*
n = 1,2,4,8,16,32

OUTPUTS:
n=1
Wall clock time = 0.013496

*/