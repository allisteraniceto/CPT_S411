#include <stdio.h>
#include <mpi.h>

#define X 100
#define Y 100
#define Z 100

void initialize_arrays(int A[X][Y][Z], int B[X][Y][Z]) {
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

    // Statically allocate memory for the 3D arrays
    int A[X][Y][Z];
    int B[X][Y][Z];
    int C[X][Y][Z];

    if (world_rank == 0) {
        initialize_arrays(A, B);
    }

    int chunk_size = X / world_size;
    int local_A[chunk_size][Y][Z];
    int local_B[chunk_size][Y][Z];
    int local_C[chunk_size][Y][Z];

    // Scatter A and B to all processes
    MPI_Scatter(A, chunk_size*Y*Z, MPI_INT, 
                local_A, chunk_size*Y*Z, MPI_INT, 
                0, MPI_COMM_WORLD);
    MPI_Scatter(B, chunk_size*Y*Z, MPI_INT, 
                local_B, chunk_size*Y*Z, MPI_INT, 
                0, MPI_COMM_WORLD);

    double startwtime, endwtime;
    startwtime = MPI_Wtime();

    for (int i = 0; i < chunk_size; i++) {
        for (int j = 0; j < Y; j++) {
            for (int k = 0; k < Z; k++) {
                local_C[i][j][k] = local_A[i][j][k] + local_B[i][j][k];
            }
        }
    }

    // Gather the results to the root process
    MPI_Gather(local_C, chunk_size*Y*Z, MPI_INT, 
               C, chunk_size*Y*Z, MPI_INT, 
               0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        endwtime = MPI_Wtime();
        print
        printf("Wall clock time = %f\n", endwtime - startwtime);
    }

    MPI_Finalize();
    return 0;
}

/*
n = 1,2,4,8,16,32

OUTPUTS:
n=1
Wall clock time = 0.013496

*/