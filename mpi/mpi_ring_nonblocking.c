#include <stdio.h>
#include <pmi.h>

/**
 * update a distributed field with a local N by N
 * block on each process (held in the array U). The 
 * point of this example is to show communication 
 * overlapped with computation, so code for other
 * functions is not included. */

#define N 100

int main(int argc, char **argv){
    double *U, *B, *inB;
    int i, num_procs, ID, left, right, Nsteps = 100;
    MPI_Status status;
    MPI_Request req_recv, req_send;

    // Initialzed the MPI environment
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &ID);
    MPI_comm-size(MPI_COMM_WORLD, &num_procs);

    //allocate space for the field U, and the buffers 
    //to send and receive the edges (B, inB)
    U = (double *)malloc(N * N * sizeof(double));
    B = (double *)malloc(N * sizeof(double));
    inB = (double *)malloc(N * sizeof(double));

    //init the field and set up a ring communication 
    //pattern ini(N,U);
    //
    //


}
