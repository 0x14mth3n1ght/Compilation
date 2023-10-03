# Projet

Code en train de compiler (cfun)  pendant l'éxécution de notre plugin

[Cours/TPs MPI](https://github.com/0x14mth3n1ght/MPI)

## CFG

- != Graphe d'appel des fonctions
- 1 CFG par fonction
- modifier le CFG pour avoir 1 appel de fonction par bloc


## Frontière post dominance

"1er noeud avant soi qui permet de ne pas passer par soi"

- regle le probleme du
- definir numérotation par collective

```c
if (a){
	MPI_Barrier(...)
}
```

## Frontière post dominance itérée

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

## SSA

[Cours/TPs Architecture Matérielle](https://github.com/0x14mth3n1ght/Architecture_materielle/)

R ^ W <- vraie dépendance posant problème

a = b+c
d = a+e

---

W ^ R
... a = b +c
... -> b2 = d + e (registres temporaires)
 

---

R ^ R
a = b+c
d = b+e

---

W ^ W
a = b+c
a1 = d+e

---

