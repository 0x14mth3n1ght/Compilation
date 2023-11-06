#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>

/*
 * Test file : we are testing a non working Allreduce
 * Adapted from the MPI Class by Julien Jaeger (CEA)
 */

#pragma ProjetCA mpicoll_check main

int main(int argc, char * argv[])
{
    MPI_Init(&argc, &argv);

    int P, N = 100;
    int me, i, sum, sum_loc = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &me);
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    for( i = 1 + me*N/P ; i <= (me+1)*N/P ; i++ ){
        sum_loc += 1;
    }
    if (me != 3){
        MPI_Allreduce(&sum_loc, &sum, 1, MPI_INT,
                      MPI_SUM, MPI_COMM_WORLD);
    }else{
        printf("I'm number 3, i'm not working today !\n");
    }

/* After reduction, all processes have, in sum_glob, the sum of N first integers
*/
    printf("1+…+%d = %d\n", N, sum);

    MPI_Finalize();
    return 1;
}
