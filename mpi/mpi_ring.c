//Ring communication test

#include "mpi.h"
#include <stdio.h>
#include <memory.h>

#define IS_ODD(x)  ((x % 2))


/*
 *  This function does the ring communication, with the odd numbered processes 
 *  sending then receiveing while the even processes receive and then send.
 *
 *  The sends are blocking sends, but this version of the ring test still is 
 *  deadlock-free since each send always has a posted receive.
 *  
 *  *x          message to shift around the ring
 *  *incoming   buffer to hold incoming message
 *  buff_count  size of message
 *  num_procs   total number of processes
 *  num_shifts  numb of times to shift message
 *  my_ID       process id number
 *
 * */
ring(double *x, double *incoming, int buff_count, int num_procs,
        int num_shifts, int my_ID){
   
    int next; /* process id of the next process*/
    int prev;

    int i;
    MPI_Status stat;

    next = (my_ID +1 ) % num_procs;
    prev = ((my_ID == 0) ? (num_procs-1) : (my_ID-1));

    printf("\nID: %d, next: %d, prev: %d \n", my_ID, next, prev);

    if(IS_ODD(my_ID)){
        for(i=0;i<num_shifts;i++){
            MPI_Send(x, buff_count, MPI_DOUBLE, next, 3, MPI_COMM_WORLD);
            MPI_Recv(incoming, buff_count, MPI_DOUBLE, prev, 3, 
                    MPI_COMM_WORLD, &stat);
        }
    }
    else{
        for(i=0;i<num_shifts;i++){
            MPI_Recv(incoming, buff_count, MPI_DOUBLE, prev, 3, 
                    MPI_COMM_WORLD, &stat);
            MPI_Send(x, buff_count, MPI_DOUBLE, next, 3, MPI_COMM_WORLD);
        }
    }
}



int main(int argc, char **argv){
    int num_shifts = 0;     //number of times to shift the message
    int buff_count = 0;     //number of double in the message
    int num_procs = 0;      //number of processes in the ring
    int ID;                 //process id
    int buff_size_bytes;    //message size in bytes
    int i;

    double t0;              //wall-clock time in seconds;
    double ring_time;       //ring comm time -this process
    double max_time;        //ring comm time -max time for all processes
    double *x;              //the outgoing message
    double *incoming;       //the incoming message

    MPI_Status stat;

    //Initialize the MPI environment 
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &ID);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    //process, test and broadcast input parameters
    if (ID == 0){
        if (argc != 3){
            printf("Usage: %s <size of message> <Num of shifts> \n", *argv);
            fflush(stdout);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        buff_count = atoi (*++argv);
        num_shifts = atoi (*++argv);
        printf(": shift %d doubles %d times \n", buff_count, num_shifts);
    }
    printf("ID: %d before bcast, buff_count: %d, num_shitfs: %d  \n", ID, buff_count, num_shifts);

    //Broadcast data from rank 0 process to all other processes
    MPI_Bcast (&buff_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast (&num_shifts, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    //Allocate space and fill the outgoing ("x") and "incoming" vectors
    buff_size_bytes = buff_count * sizeof(double);

    x = (double *) malloc(buff_size_bytes);
    incoming = (double *) malloc(buff_size_bytes);

    for(i=0;i<buff_count;i++){
        x[i] = (double) i;
        incoming[i] = -1.0;
    }

    //Do the ring communication tests
    MPI_Barrier (MPI_COMM_WORLD);
    printf("ID: %d over barrier, buff_count: %d, num_shitfs: %d  \n", ID, buff_count, num_shifts);
    t0 = MPI_Wtime ();
    /* code to pass message around ring */
    ring (x,incoming,buff_count,num_procs,num_shifts,ID);
    ring_time = MPI_Wtime() -t0;

    //Analyze results
    
    MPI_Reduce (&ring_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if(ID == 0){
        printf("\nRing test took %f seconds \n", max_time);
    }
    MPI_Finalize();


}
