#include <stdio.h>
#include "memory.h"
#include "mpi.h"

#define ROW  16       //16 row
#define GRID ROW*ROW  //grid 16*16


int main(int argc, char **argv){
    int Tag1 = 1;
    int Tag2 = 2;            //msg tags
    int num_procs;           //number of process in the group
    int ID;                  //a unique identifier ranging from 0 to (num_procs-1)
    int buffer_count = 10;   //number of items in the message to bounce
    long *buffer;            //buffer to bounce between processes
    int i;
    
    int array_len;              //length of array in byte
    int *grid;               //dynamic allocate array
    
    MPI_Status stat;         //MPI status parameter
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &ID);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    //if(num_procs !=2 ) MPI_Abort(MPI_COMM_WORLD,1);
    
    if(ID == 0){
        /* the first procs need to print the whole grid*/
        array_len = GRID * sizeof(int) << 3; 
        grid = (int *)malloc(array_len);
    }
    else{
        array_len = GRID / num_procs * sizeof(int) <<3;
        grid=(int *)malloc(GRID / num_procs * sizeof(int) << 3 );
    }
    memset(grid, 0, array_len); //init to 0
    







    buffer = (long *)malloc(buffer_count * sizeof(long));
    
    for(i=0; i<buffer_count;i++){
        buffer[i] = (long) i;
    }
    if(ID == 0){
        MPI_Send(buffer, buffer_count,MPI_LONG,1,Tag1,MPI_COMM_WORLD);
        MPI_Recv(buffer, buffer_count,MPI_LONG,1,Tag2,MPI_COMM_WORLD,&stat);
    }
    else{
        MPI_Recv(buffer,buffer_count,MPI_LONG,0,Tag1,MPI_COMM_WORLD,&stat);
        for(i=0; i<buffer_count; i++){
            printf("MPI_Recv: %ld \n", buffer[i]);
        }
        MPI_Send(buffer,buffer_count,MPI_LONG,0,Tag2,MPI_COMM_WORLD);
    }

    MPI_Finalize();

}
