#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define f(x) ((x)*(x))

struct ParallelTrapezoidalRule {
    double a;
    double b;
    int n;
    double h;
    double c1; // Constant for serial runtime
    double c2; // Constant for overhead
};

double t_serial(int n) {
    // Simulate serial runtime as a function of n
    return n * 1e-6; // Example: 1 microsecond per trapezoid
}

double t_parallel(int n, int p) {
    // Simulate parallel runtime as a function of n and p
    return t_serial(n) / p + overhead_function(p);
}

double overhead_function(int p) {
    // Simulate overhead as a function of p
    return p * 1e-4; // Example: 0.1 milliseconds per process
}

double speedup(int n, int p) {
    return t_serial(n) / t_parallel(n, p);
}

double efficiency(int n, int p) {
    return speedup(n, p) / p;
}

int main(int argc, char *argv[]){
    MPI_Init(&argc, &argv);
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    double a = 0.0;     // Lower limit of integration
    double b = 1.0;     // Upper limit of integration
    int n = 100;       // Number of trapezoids
    double h = (b - a) / n; // Width of each trapezoid

    // to time
    double startwtime = 0.0, endwtime;
    startwtime = MPI_Wtime();

    // calculate range for each process
    int local_n = n / world_size;
    double local_a = a + world_rank * local_n * (b - a) / n;
    double local_b = local_a + local_n * (b - a) / n;
  
    double local_integral = (f(a) + f(b)) / 2.0;
    // Sum the areas of each trapezoid
    for (int i = 1; i < local_n; i++) {
        double x = a + i * h;
        local_integral += f(x);
    }
    local_integral *= h;

    // reduce all the local integral to the root process

    double global_integral;
    MPI_Reduce(&local_integral, &global_integral, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  
    if (world_rank == 0){
        endwtime = MPI_Wtime();
        printf("The integral from %.2f to %.2f is approximately %.16f\n", a, b, local_integral);
        printf("wall clock time = %f\n", endwtime - startwtime);
    }
    MPI_Finalize();
    return 0;
}
