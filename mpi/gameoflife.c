#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "mpi.h"

#define DIM 16
#define GRID DIM*DIM
#define IS_ODD(x)  ((x % 2))

#define IS_BIT_SET(char_array,i) (char_array[i/8]>>(8-(i%8)-1) & 1)
#define SET_BIT(char_array,i) (char_array[i/8] |= 1<<(8-(i%8)-1) )
#define OFF_BIT(char_array,i) (char_array[i/8] &= ~(1<<(8-(i%8)-1)) )
#define MOD(x,y) ((x%y)<0 ? (x%y+y) : (x%y) )

const int glider[ 256 ] = { 
  0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
  1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };


int num_procs;      //process number
int ID;             //process id


void print_bit(char *bytes, int len, int ID){
  int i;
  int j;
  printf("ID=%d ====================\n",ID);
  for(i=0; i<len; i++){
    for(j=7; j>=0; j--){
      char mask = 1<<j;
      if(bytes[i]&mask)
        printf("%d ",1);
      else 
        printf("%d ",0);
    }
    if((i+1)%2 == 0)
      printf("\n");
  }

}
/* Init grid of each process 
 * Process 0            need DIM*DIM
 * Process 1            need DIM*2 (row1 row2)
 * Process 2 to (n-1)   need DIM*3 (row(i-1) row(i) row(i+1))
 * process n            need DIM*2 (row(n-1) row(n))
 *
 * */
void init_procs(char* grid, int ID, int row_per_id, int row_num){
  int tmp;
  int i, j, k;
  if(ID == 0)
    k = 0;
  else
    k = ID * row_per_id - 1;
  for(i=0;i<row_num;i++){
    for(j=0;j<DIM;j++){
      if(glider[(k+i) % DIM * DIM + j] == 1){
        tmp = i * DIM + j;
        grid[tmp/8]|= (1<<(8-(tmp%8)-1));       
      }
    }
  }
}

/**
 * update grid
 */
void update(char *grid,int num_row,int row_per_id){

  int i, j;       // i starting row, j starting col
  int count = 0;  //num of live nighbour

  //i_max max row of grid that this process update
  int i_max = ID==0 ? (row_per_id) :(num_row-1);
  int i_start = ID==0 ? 0:1; 
  i = i_start;

  //temp space for update grid
  int tmp_grid_len = (i_max-i) * DIM / 8;
  char *tmp_grid = (char *)malloc(tmp_grid_len);
  memcpy(tmp_grid,(grid + i * DIM / 8),tmp_grid_len);
  //printf("ID:%d, tmp_len:%d \n",ID, tmp_grid_len);
  int k = 0;
  for(;i<i_max;i++){
    for(j=0; j<DIM; j++){
      count = num_live(grid,i,j);
      if(IS_BIT_SET(grid,(i*DIM+j) )){
        if(!(count==2 || count == 3))
          OFF_BIT(tmp_grid,(k*DIM+j));
      }
      else{
        if(count == 3)
          SET_BIT(tmp_grid,(k*DIM+j));
      }
    }//end of inner for
    k++;
  }//enf of out for
  memcpy((grid + i_start * DIM /8),tmp_grid,tmp_grid_len);
  free(tmp_grid);
}

int valid(int row, int col){
  if(row <0 || col <0 || row >= DIM || col >= DIM)
    return 0;
  return 1;
}

int num_live(char *grid, int row, int col){
  int count = 0;
  int i, j;
  for(i=-1; i<2; i++){
    for(j=-1;j<2;j++){
      int tmp_row = MOD((row+i),DIM);
      int tmp_col = MOD((col+j),DIM);
      if(!(i==0&&j==0)
          && IS_BIT_SET(grid,(tmp_row*DIM+tmp_col) ) ){
        count++;
      }
    }
  }
  return count;
}

/*
 * Used by process 0 to receive grid from other processes
 * 
 * grid         grid of process 0
 * row_per_id   num of row each process keep
 * stat         MPI_Status
 *
 * */
void recv_from_other(char *grid, int row_per_id,MPI_Status stat){
  int i;
  int row_len = DIM/8;
  for(i=1;i<num_procs;i++){
    //recv from process i
    MPI_Recv(grid + i * row_len * row_per_id,
        row_len * row_per_id, MPI_CHAR,
        i,1,MPI_COMM_WORLD,&stat);
  }
}

/*
 * Used by process i to send the part of
 * grid them response for update to process i 
 * 
 * i            process i to send to
 * grid         grid of process i
 * row_per_id   num of row each process keep
 * stat         MPI_Status
 *
 * */

void send_to_process(int i, char *grid, int row_per_id,MPI_Status stat){
  int row_len=DIM/8;
  int off_set=ID==0 ? off_set=0:row_len;
  MPI_Send(grid+off_set,row_len*row_per_id,MPI_CHAR,i,1,MPI_COMM_WORLD);
}

/**
 * send boundary row to neighbour process. 
 *  
 * for odd process:
 * odd send to pre
 * odd rev from pre
 * odd send to next
 * odd rev from next
 * 
 * for even process: 
 * even rcv from next
 * even send to next
 * even rcv from prev
 * even send to prev
 */
void send_to_neighbour(char *grid,int num_row, int row_per_id, MPI_Status stat){
  if(IS_ODD(ID)){
    /*commuctae with prev process*/
    //send  my row to ID-1
    MPI_Send(grid+DIM/8,DIM/8,MPI_CHAR,
        ID-1,11,MPI_COMM_WORLD);
    //recv from ID-1
    MPI_Recv(grid,DIM/8,MPI_CHAR,
        ID-1,2,MPI_COMM_WORLD,&stat);

    /*communicate with next process*/
    //send my row to ID+1
    MPI_Send(grid+(num_row-2)*DIM/8,DIM/8,
        MPI_CHAR,(ID+1)%num_procs,21,MPI_COMM_WORLD);
    //recv from ID+1
    MPI_Recv(grid+(num_row-1)*DIM/8,DIM/8,
        MPI_CHAR,(ID+1)%num_procs,3,MPI_COMM_WORLD,
        &stat);
  }
  else{/*even*/
    /*communicate with next */
    int off_set = ID==0 ? row_per_id+1:
      num_row;
    //rev from ID+1
    MPI_Recv(grid+(off_set-1)*DIM/8,DIM/8,
        MPI_CHAR,ID+1,11,MPI_COMM_WORLD,
        &stat);
    //send to ID+1
    MPI_Send(grid+(off_set-2)*DIM/8,DIM/8,
        MPI_CHAR,ID+1,2,MPI_COMM_WORLD);

    /*communicate with  prev*/
    char *recv_off_set = 0;
    char * send_off_set = 0;
    int prev = 0;
    if(ID==0){
      recv_off_set = grid + (DIM - 1) * DIM/8;
      send_off_set = grid;
      prev = num_procs-1;
    }
    else{
      recv_off_set = grid;
      send_off_set = grid + DIM/8;
      prev = ID -1;
    }
    //rev from ID-1
    MPI_Recv(recv_off_set,DIM/8,
        MPI_CHAR,prev,21,MPI_COMM_WORLD,
        &stat);
    //send to ID-1
    MPI_Send(send_off_set,DIM/8,
        MPI_CHAR,prev,3,MPI_COMM_WORLD);
  }
}

int main(int argc, char *argv[]){
  int row_per_id;     //num of row each process cacl
  int num_row;        //num of row each process need
  //to calc change
  char *grid;         //partial grid each process store
  int len;            //num of char  of partial grid
  char *buff;         //send and recv buff
  int buf_len;        //buf length (num of char)

  MPI_Request snd_req; // send handle
  MPI_Request rcv_req; //receive handle
  MPI_Status stat;


  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&ID);
  MPI_Comm_size(MPI_COMM_WORLD,&num_procs);

  assert(DIM%num_procs == 0 && DIM%8==0);
  row_per_id = DIM/num_procs;

  /* process 0 keep all row
   *
   * process 1 to n-1 keep DIM/n row, plus
   * upper boundary row, and lower boundary row
   * */
  if(ID == 0) 
    num_row = DIM;
  else 
    num_row = row_per_id+2;

  len = DIM * num_row >> 3;
  grid = (char *) malloc(len);

  memset(grid,0,len);//init to all zero
  init_procs(grid,ID,row_per_id,num_row);

  //iteration
  int itr;
  for(itr=0;itr<64;itr++){ 
    //update
    update(grid,num_row,row_per_id);

    if(num_procs>1){
      //send to neighour process
      send_to_neighbour(grid,num_row,row_per_id,stat);
      //recv from other process
      if(ID==0){
        recv_from_other(grid,row_per_id,stat);
      }
      //send to process 0
      else{
        send_to_process(0,grid,row_per_id,stat);
      }
    }//end of num_process > 1
    if(ID==0){
      printf("i=%d ===================\n",itr);
      print_bit(grid,len,ID);
      printf("\n");
    }
    MPI_Barrier(MPI_COMM_WORLD);
  }
  return 0;
}
