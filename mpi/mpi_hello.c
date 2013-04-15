/*C Example*/

#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[]){
    int num_proces,id;
    if(MPI_Init(&argc,&argv) != MPI_SUCCESS){printf("MPI_Init() error \n");} /*starts MPI*/
    MPI_Comm_rank(MPI_COMM_WORLD, &id);/* get current process id */
    MPI_Comm_size(MPI_COMM_WORLD,&num_proces); /* get number of processes*/
    printf("Hello world form process %d of %d \n",id,num_proces);
    MPI_Finalize();
    return 0;

}
