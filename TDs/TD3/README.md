## Q1

Se servir de la correction de la Q8 (callee_name) pour repérer le nom dans gimple
Ouvrir MPI_Collectives.def et faire un strcmp pour chaque nom de fnct

Faire la boucle dans execute pour prendre en argument un `gimple *stmt`

```bash
[GRAPHVIZ] Generating CFG of function main in file <main_test2.c_26_0_ini.dot>
~/gcc12/bin/g++ -I`~/gcc12/bin/gcc -print-file-name=plugin`/include -g -Wall -fno-rtti -shared -fPIC  -o libplugin_TP3_1.so plugin_TP3_1.cpp
OMPI_MPICC=~/gcc12/bin/gcc mpicc test2.c -g -O3 -o TP3_1 -fplugin=./libplugin_TP3_1.so 
Function: 'mpi_call'
        MPI COLLECTIVE: 'MPI_Barrier'
        MPI COLLECTIVE: 'MPI_Barrier'
        MPI COLLECTIVE: 'MPI_Barrier'
        MPI COLLECTIVE: 'MPI_Barrier'
Function: 'main'
        MPI COLLECTIVE: 'MPI_Init'
        MPI COLLECTIVE: 'MPI_Finalize'
```

![mpi_call_test_0](./mpi_call_test2.c_7_0_ini.png)

![main_26_0](./main_test2.c_26_0_ini.png)

**Pour main**:

- Souce : BB0

- BB2 : MPI_Init() + déclaration des int

- BB4 : for()

- BB3 : boucle sur lui même: contenu du for

- BB5: sortie de boucle : printf & MPI_finalize()

- BB1: puits

## Q2

Pareil mais n'afficher uniquement que les fonctions MPI

```bash
~/gcc12/bin/g++ -I`~/gcc12/bin/gcc -print-file-name=plugin`/include -g -Wall -fno-rtti -shared -fPIC  -o libplugin_TP3_2.so plugin_TP3_2.cpp
OMPI_MPICC=~/gcc12/bin/gcc mpicc test2.c -g -O3 -o TP3_2 -fplugin=./libplugin_TP3_2.so 
Function: 'mpi_call'
        MPI COLLECTIVE: 'MPI_Barrier'
        MPI COLLECTIVE: 'MPI_Barrier'
        MPI COLLECTIVE: 'MPI_Barrier'
        MPI COLLECTIVE: 'MPI_Barrier'
Function: 'main'
        MPI COLLECTIVE: 'MPI_Init'
        MPI COLLECTIVE: 'MPI_Finalize'
```

## Q3

Pareil mais avec un enum : renvoyer la valeur de la collective dans l'enum

```bash
~/gcc12/bin/g++ -I`~/gcc12/bin/gcc -print-file-name=plugin`/include -g -Wall -fno-rtti -shared -fPIC  -o libplugin_TP3_3.so plugin_TP3_3.cpp
OMPI_MPICC=~/gcc12/bin/gcc mpicc test2.c -g -O3 -o TP3_3 -fplugin=./libplugin_TP3_3.so 
Function: 'mpi_call'
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
Function: 'main'
        MPI COLLECTIVE: 'MPI_Init' (code: 0)
        MPI COLLECTIVE: 'MPI_Finalize' (code: 1)
```

## Q4


## Q5

```bash
~/gcc12/bin/g++ -I`~/gcc12/bin/gcc -print-file-name=plugin`/include -g -Wall -fno-rtti -shared -fPIC  -o libplugin_TP3_5.so plugin_TP3_5.cpp
OMPI_MPICC=~/gcc12/bin/gcc mpicc test2.c -g -O3 -o TP3_5 -fplugin=./libplugin_TP3_5.so 
Function: 'mpi_call'
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        BB index: 2
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        BB index: 3
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        BB index: 3
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        BB index: 4
BB index: 2
BB aux:   0
BB index: 3
BB aux:   0
BB index: 4
BB aux:   0
BB index: 5
BB aux:   0
Function: 'main'
        MPI COLLECTIVE: 'MPI_Init' (code: 0)
        BB index: 2
        MPI COLLECTIVE: 'MPI_Finalize' (code: 1)
        BB index: 5
BB index: 2
BB aux:   0
BB index: 3
BB aux:   0
BB index: 4
BB aux:   0
BB index: 5
BB aux:   0
```

## Q6

```bash
~/gcc12/bin/g++ -I`~/gcc12/bin/gcc -print-file-name=plugin`/include -g -Wall -fno-rtti -shared -fPIC  -o libplugin_TP3_6.so plugin_TP3_6.cpp
OMPI_MPICC=~/gcc12/bin/gcc mpicc test2.c -g -O3 -o TP3_6 -fplugin=./libplugin_TP3_6.so 
Function: 'mpi_call'
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        BB index: 2
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        BB index: 3
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        BB index: 3
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        BB index: 4
[GRAPHVIZ] Generating CFG of function mpi_call in file <mpi_call_test2.c_7_mpi_call.dot>
Function: 'main'
        MPI COLLECTIVE: 'MPI_Init' (code: 0)
        BB index: 2
        MPI COLLECTIVE: 'MPI_Finalize' (code: 1)
        BB index: 5
[GRAPHVIZ] Generating CFG of function main in file <main_test2.c_26_main.dot>
```

![mpi_call_7](./mpi_call_test2.c_7_mpi_call.png)

![main_26][./main_test2.c_26_main.png]

## Q7

```bash
~/gcc12/bin/g++ -I`~/gcc12/bin/gcc -print-file-name=plugin`/include -g -Wall -fno-rtti -shared -fPIC  -o libplugin_TP3_7.so plugin_TP3_7.cpp
OMPI_MPICC=~/gcc12/bin/gcc mpicc test2.c -g -O3 -o TP3_7 -fplugin=./libplugin_TP3_7.so 
Function: 'mpi_call'
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        BB index: 2
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        BB index: 3
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        BB index: 3
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        BB index: 4
[GRAPHVIZ] Generating CFG of function mpi_call in file <mpi_call_test2.c_7_mpi_call.dot>
        Function has at least one BB with 2+ MPI Collectives: 1
Function: 'main'
        MPI COLLECTIVE: 'MPI_Init' (code: 0)
        BB index: 2
        MPI COLLECTIVE: 'MPI_Finalize' (code: 1)
        BB index: 5
[GRAPHVIZ] Generating CFG of function main in file <main_test2.c_26_main.dot>
        Function has at least one BB with 2+ MPI Collectives: 0
```

![mpi_call_7.2](./mpi_call_test2.c_7_mpi_call.png)

![main_26.2](./main_test2.c_26_main.png)

## Q8

```bash
~/gcc12/bin/g++ -I`~/gcc12/bin/gcc -print-file-name=plugin`/include -g -Wall -fno-rtti -shared -fPIC  -o libplugin_TP3_8.so plugin_TP3_8.cpp
OMPI_MPICC=~/gcc12/bin/gcc mpicc test2.c -g -O3 -o TP3_8 -fplugin=./libplugin_TP3_8.so 
Function: 'mpi_call'
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        Split the block 03
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
[GRAPHVIZ] Generating CFG of function mpi_call in file <mpi_call_test2.c_7_mpi_call.dot>
Function: 'main'
        MPI COLLECTIVE: 'MPI_Init' (code: 0)
        MPI COLLECTIVE: 'MPI_Finalize' (code: 1)
[GRAPHVIZ] Generating CFG of function main in file <main_test2.c_26_main.dot>
rm libplugin_TP3_6.so libplugin_TP3_1.so libplugin_TP3_7.so libplugin_TP3_5.so libplugin_TP3_1_old.so libplugin_TP3_2.so libplugin_TP3_3.so libplugin_TP3_8.so
```

![mpi_call_7.3](./mpi_call_test2.c_7_mpi_call.png)

![main_26.3](./main_test2.c_26_main.png)
