#include<stdio.h>
#include<stdlib.h>

#include <mpi.h>

/*
 * Test file : we are testing how the pragmas are working
 */

#pragma ProjetCA mpicoll_check (foo, unexisting_function,,)

int foo(){
    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank != 1){
        MPI_Barrier(MPI_COMM_WORLD);
    }else{
        printf("Im the main only the workers should wait !\n");
    }
    return 5;
}

int main(int argc, char * argv[]){

    MPI_Init(&argc, &argv);
    printf("Hello world : this is the output of foo : %d \n", foo());
    MPI_Finalize();
    return 0;
}
