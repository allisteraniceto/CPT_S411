/*  Cpt S 411, Introduction to Parallel Computing
 *  School of Electrical Engineering and Computer Science
 *    
 *  Example code
 *  	Hello world MPI
 *          
 */
#include <stdio.h>
#include <mpi.h> 

int main(int argc,char *argv[])
{

   MPI_Init(&argc,&argv);
   int world_rank, world_size, number;
   MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
   MPI_Comm_size(MPI_COMM_WORLD, &world_size);

   
   if (world_rank == 0) {
      number = 10;
      MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
      MPI_Recv(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
       printf("Process 0 received new value %d from process 1\n", number);
     

   } else if (world_rank == 1) {
      MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
      printf("Process 1 received number %d from process 0\n", number);
      number+=10;
      MPI_Send(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
   }

   MPI_Finalize();
}
