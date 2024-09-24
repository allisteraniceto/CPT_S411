#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define N 1000 // Total number of integers
int main(int argc, char * argv[]) {
  int numprocs, myid, i;
  int * data = NULL, * sub_array = NULL;
  int local_sum = 0, total_sum = 0, recv_sum;
  int elements_per_proc;
  // Initialize MPI environment
  MPI_Init( & argc, & argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  elements_per_proc = N / numprocs;
  if (myid == 0) {
    // Process 0 initializes an array of random integers gaming gamers😎
    data = (int * ) malloc(N * sizeof(int));
    srand(time(NULL));
    for (i = 0; i < N; i++) {
      data[i] = rand() % 100;
    }
    printf("Data initialized by process 0\n");
  }
  // Each process will have a sub-array of size elements_per_proc
  sub_array = (int * ) malloc(elements_per_proc * sizeof(int));
  // Process 0 sends sub-arrays to other processes using MPI_Send/MPI_Recv
  if (myid == 0) {
    for (i = 1; i < numprocs; i++) {
      MPI_Send( & data[i * elements_per_proc], elements_per_proc, MPI_INT, i,0, MPI_COMM_WORLD);
    }
    // Process 0 keeps the first sub-array
    for (i = 0; i < elements_per_proc; i++) {
      sub_array[i] = data[i];
    }
  } else {
    // Other processes receive their part of the array
    MPI_Recv(sub_array, elements_per_proc, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  // Each process calculates its partial sum
  for (i = 0; i < elements_per_proc; i++) {
    local_sum += sub_array[i];
  }
  // Print out the partial sum for each process
  printf("Partial sum from processor %d of total %d processors is: %d\n", myid,
    numprocs, local_sum);
  if (myid != 0) {
    // Each process sends its partial sum back to process 0
    MPI_Send( & local_sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    printf("Send %d to processor 0 by processor %d\n", local_sum, myid);
  } else {
    // Process 0 receives the partial sums from all other processes
    total_sum = local_sum; // Include process 0's own partial sum
    for (i = 1; i < numprocs; i++) {
      MPI_Recv( & recv_sum, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      printf("Received %d from processor %d\n", recv_sum, i);
      total_sum += recv_sum;
    }
    // Print the total sum
    printf("Total sum is %d\n", total_sum);
  }
  // Clean up
  free(sub_array);
  if (myid == 0) {
    free(data);
  }
  MPI_Finalize();
  return 0;
}