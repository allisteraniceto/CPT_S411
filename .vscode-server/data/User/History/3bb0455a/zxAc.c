#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 100  // Size of the 3D arrays

void initialize_arrays(int *A, int *B) {
    for (int i = 0; i < N * N * N; i++) {
        A[i] = 1;  // Initialize A with 1
        B[i] = 2;  // Initialize B with 2
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (N % size != 0) {
        if (rank == 0) {
            printf("The array size (%d) is not divisible by the number of processes (%d).\n", N, size);
        }
        MPI_Finalize();
        return 1;
    }
    
    int chunk_size = N / size;  // each process works on chunk_size rows of the 3D array

    // allocate memory for local chunks (3D array but linear since MPI can only work with 1D arrays)
    int *local_A = (int *)malloc(chunk_size * N * N * sizeof(int));
    int *local_B = (int *)malloc(chunk_size * N * N * sizeof(int));
    int *local_C = (int *)malloc(chunk_size * N * N * sizeof(int));
    
    // Full arrays are allocated only on the root process (rank 0)
    int *A = NULL;
    int *B = NULL;
    int *C = NULL;
    
    if (rank == 0) {
        A = (int *)malloc(N * N * N * sizeof(int));
        B = (int *)malloc(N * N * N * sizeof(int));
        C = (int *)malloc(N * N * N * sizeof(int));
        initialize_arrays(A, B);
    }
    
    double start_time = MPI_Wtime();  // Start measuring time
    
    // Scatter the chunks of arrays A and B to all processes
    MPI_Scatter(A, chunk_size * N * N, MPI_INT, 
                local_A, chunk_size * N * N, MPI_INT, 
                0, MPI_COMM_WORLD);
    MPI_Scatter(B, chunk_size * N * N, MPI_INT, 
                local_B, chunk_size * N * N, MPI_INT, 
                0, MPI_COMM_WORLD);
    
    // Perform the addition of the local chunks in parallel
    for (int i = 0; i < chunk_size * N * N; i++) {
        local_C[i] = local_A[i] + local_B[i];
    }
    
    // Gather the local results back to the root process
    MPI_Gather(local_C, chunk_size * N * N, MPI_INT, 
               C, chunk_size * N * N, MPI_INT, 
               0, MPI_COMM_WORLD);
    
    double end_time = MPI_Wtime();  // End measuring time
    
    // Output the execution time
    if (rank == 0) {
        printf("n=%d\n", size);
        printf("Execution time with %d processes: %f seconds\n", size, end_time - start_time);
    }
    
    // Free allocated memory
    free(local_A);
    free(local_B);
    free(local_C);
    
    if (rank == 0) {
        free(A);
        free(B);
        free(C);
    }
    
    MPI_Finalize();
    return 0;
}

/*
n=1,2,4,8,16,32


*/