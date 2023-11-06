#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>
/*
 * Test file : we don't understand this one !
 * If we look at the dot graph, we shouldn't have any warning since
 * there are exactly two barriers in each pathways
 * but we have a deadlock warning nonetheless
 */

#pragma ProjetCA mpicoll_check main

int main(int argc, char * argv[])
{
    MPI_Init(&argc, &argv);

    volatile int a = 0;

    if(a * a > 0){
        a++;
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
    }else{
        a--;
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 1;
}