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
    
    //calculate chunk size for each process
    int chunk_size = (N * N * N + size - 1) / size;  // round up to handle remainder
    
    // allocate memory for local chunks
    int *local_A = (int *)malloc(chunk_size * sizeof(int));
    int *local_B = (int *)malloc(chunk_size * sizeof(int));
    int *local_C = (int *)malloc(chunk_size * sizeof(int));
    
    // initialize array but only on the root process
    int *A = NULL;
    int *B = NULL;
    int *C = NULL;
    
    if (rank == 0) {
        A = (int *)malloc(N * N * N * sizeof(int));
        B = (int *)malloc(N * N * N * sizeof(int));
        C = (int *)malloc(N * N * N * sizeof(int));
        initialize_arrays(A, B);
    }
    
    double start_time = MPI_Wtime();  // start time
    
    // scatter the chunks of arrays A and B to all processes
    MPI_Scatter(A, chunk_size, MPI_INT, 
                local_A, chunk_size, MPI_INT, 
                0, MPI_COMM_WORLD);
    MPI_Scatter(B, chunk_size, MPI_INT, 
                local_B, chunk_size, MPI_INT, 
                0, MPI_COMM_WORLD);
    
    // perform the addition of the local chunks in parallel
    for (int i = 0; i < chunk_size; i++) {
        local_C[i] = local_A[i] + local_B[i];
    }
    
    // gather the local results back to the root process
    MPI_Gather(local_C, chunk_size, MPI_INT, 
               C, chunk_size, MPI_INT, 
               0, MPI_COMM_WORLD);
    
    double end_time = MPI_Wtime();  // end time
    
    // Output the execution time
    if (rank == 0) {
        printf("n=%d\n", size);
        printf("Execution time with %d processes: %f seconds\n", size, end_time - start_time);
        free(A);
        free(B);
        free(C);
    }
    
    // Free allocated memory
    free(local_A);
    free(local_B);
    free(local_C);
    
    MPI_Finalize();
    return 0;
}

/*
n=1,2,4,8,16,32

OUTPUTS:
n=1
Execution time with 1 processes: 0.021317 seconds
n=2
Execution time with 2 processes: 0.017301 seconds
n=4
Execution time with 4 processes: 0.015731 seconds
n=8
Execution time with 8 processes: 0.015421 seconds
n=16
Execution time with 16 processes: 0.125536 seconds
n=32
Execution time with 32 processes: 0.304975 seconds
*/