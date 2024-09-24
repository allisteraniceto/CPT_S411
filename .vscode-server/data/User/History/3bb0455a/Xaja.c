#include <stdio.h>
#include <mpi.h>

#define N 100

void initialize_arrays(int A[N][N][N], int B[N][N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                A[i][j][k] = 1; // Example initialization
                B[i][j][k] = 2; // Example initialization
            }
        }
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int chunk_size = N / size;
    int start = rank * chunk_size;
    int end = start + chunk_size;

    int A[N][N][N];
    int B[N][N][N];
    int C[N][N][N];

    if (rank == 0) {
        initialize_arrays(A, B);
    }

    int local_A[chunk_size][N][N];
    int local_B[chunk_size][N][N];
    int local_C[chunk_size][N][N];

    // Scatter A and B to all processes
    MPI_Scatter(A, chunk_size*N*N, MPI_INT, 
                local_A, chunk_size*N*N, MPI_INT, 
                0, MPI_COMM_WORLD);
    MPI_Scatter(B, chunk_size*N*N, MPI_INT, 
                local_B, chunk_size*N*N, MPI_INT, 
                0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD); // Synchronize all processes

    double start_time = MPI_Wtime();

    // Perform addition in parallel
    for (int i = 0; i < chunk_size; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                local_C[i][j][k] = local_A[i][j][k] + local_B[i][j][k];
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD); // Synchronize all processes

    double end_time = MPI_Wtime();

    // Gather the results to the root process
    MPI_Gather(local_C, chunk_size*N*N, MPI_INT, 
               C, chunk_size*N*N, MPI_INT, 
               0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Wall clock time = %f\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}