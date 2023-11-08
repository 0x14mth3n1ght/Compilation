## Rapport et présentation

- Présentation: penser plus tard à **penser au client, moins technique, plus contextuel avec les démos** (live ou non)

- Par le même occasion: pas forcément utile d'inclure les bouts de code -> se concentrer sur la théorie et les fonctionnalités

- Voir `Rapport_CA.pdf` et `pres/index.html`


## Code et réalisation du projet

- Partie (2) **pragmas** : partie faite, voir [GPT](https://chat.openai.com/share/63a3b18b-a2a8-40fc-bf02-38bdd7db5c26) pour le raisonnement

Voir `test/test-mpi-foo.c`

- Partie (1) **warnings** (PDF+) à finir: voir [GPT](https://chat.openai.com/share/e3b682bd-8cd2-44c2-8637-e2bf3c46a00e) pour le raisonnement

### Frontière post dominance -> faite

Voir `test/test-mpi-1.c`, `test/test-mpi-2.c`

"1er noeud avant soi qui permet de ne pas passer par soi"

- regle le probleme du
- definir numérotation par collective

```c
if (a){
        MPI_Barrier(...)
}
```

### Frontière post dominance itérée -> à finir

Voir `test/test-mpi-rank-allreduce.c`  `test/test-mpi-two-barriers.c`

- règle ce problème (assigner un rang à chaque collective)

```c
if (a){
        MPI_Barrier()
        //MPI_Send()
}
if (b){
        //
}
```

```c
if (a){
        MPI_Barrier()
}
else{
        MPI_Reduce()
        MPI_Barrier()
}
```

## Installer GCC12

```bash
wget https://ftp.gnu.org/gnu/gcc/gcc-12.2.0/gcc-12.2.0.tar.gz
tar -xvf gcc-12.2.0.tar.gz
mkdir gcc-12.2.0/build; cd gcc-12.2.0/build 
../configure --prefix=~/gcc-12.2.0 --enable-languages=c,c++,fortran --enable-plugin --disable-bootstrap --disable-multilib
make
make install
~/gcc-12.2.0/bin/gcc
```

## Remark.js to PDF

https://github.com/gnab/remark/issues/50

```bash
yay -S wkhtmltopdf
wkhtmltopdf --page-width 111 --page-height 148  -O "Landscape"  gnab.github.io/remark remark.pdf
```

## Correction

- https://gitlab.com/Nitorac/mpi-plugin-ca/
