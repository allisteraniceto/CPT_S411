#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define X 100
#define Y 100
#define Z 100

//initialize arrays A and B
void initialize_arrays(int A[X][Y][Z], int B[X][Y][Z]) {
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

    double A[X][Y][Z], B[X][Y][Z], C[X][Y][Z];
    double startwtime, endwtime;

    if (world_rank == 0) {
        initialize_arrays(A, B);
    }

    MPI_Bcast(A, X*Y*Z, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(B, X*Y*Z, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    int chunk_size = X / world_size;
    int start = world_rank * chunk_size;
    int end = (world_rank + 1) * chunk_size;

    startwtime = MPI_Wtime();

    for (int i = start; i < end; i++) {
        for (int j = 0; j < Y; j++) {
            for (int k = 0; k < Z; k++) {
                C[i][j][k] = A[i][j][k] + B[i][j][k];
            }
        }
    }

    MPI_Gather(C + start * Y * Z, chunk_size * Y * Z, MPI_DOUBLE, C, chunk_size * Y * Z, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        endwtime = MPI_Wtime();
        printf("wall clock time = %f\n", endwtime - startwtime);
    }

    MPI_Finalize();
    return 0;
}