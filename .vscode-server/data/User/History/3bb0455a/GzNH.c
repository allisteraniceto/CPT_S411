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
    
    // calculate the size of chunks for each process
    int base_chunk_size = N / size;  
    int remainder = N % size;        // need to handle case when N is not divisible by sizes
    
    // each process gets base_chunk_size rows, and the first `remainder` processes get 1 extra row
    int start = rank * base_chunk_size + (rank < remainder ? rank : remainder);
    int chunk_size = base_chunk_size + (rank < remainder ? 1 : 0);
    
    // allocate memory for local chunks
    int *local_A = (int *)malloc(chunk_size * N * N * sizeof(int));
    int *local_B = (int *)malloc(chunk_size * N * N * sizeof(int));
    int *local_C = (int *)malloc(chunk_size * N * N * sizeof(int));
    
    // full arrays are allocated only on the root process (rank 0)
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
    
    // Create arrays for counts and displacements
    int *counts = (int *)malloc(size * sizeof(int));
    int *displs = (int *)malloc(size * sizeof(int));
    
    // Fill counts and displacements for each process
    for (int i = 0; i < size; i++) {
        counts[i] = ((N / size) + (i < remainder ? 1 : 0)) * N * N;  // Size of each chunk
        displs[i] = (i * base_chunk_size + (i < remainder ? i : remainder)) * N * N;  // Start index
    }
    
    // Scatter the chunks of arrays A and B to all processes
    MPI_Scatterv(A, counts, displs, MPI_INT, 
                 local_A, chunk_size * N * N, MPI_INT, 
                 0, MPI_COMM_WORLD);
    MPI_Scatterv(B, counts, displs, MPI_INT, 
                 local_B, chunk_size * N * N, MPI_INT, 
                 0, MPI_COMM_WORLD);
    
    // Perform the addition of the local chunks in parallel
    for (int i = 0; i < chunk_size * N * N; i++) {
        local_C[i] = local_A[i] + local_B[i];
    }
    
    // Gather the local results back to the root process
    MPI_Gatherv(local_C, chunk_size * N * N, MPI_INT, 
                C, counts, displs, MPI_INT, 
                0, MPI_COMM_WORLD);
    
    double end_time = MPI_Wtime();  // End measuring time
    
    // Output the execution time
    if (rank == 0) {
        printf("Execution time with %d processes: %f seconds\n", size, end_time - start_time);
        free(A);
        free(B);
        free(C);
    }
    
    // Free allocated memory
    free(local_A);
    free(local_B);
    free(local_C);
    free(counts);
    free(displs);
    
    MPI_Finalize();
    return 0;
}

/*
n=1,2,4,8,16,32
OUTPUTS:

*/
