/* VOICU Alex-Georgian  */

/*-- TCoada.h --- tipul coada ---*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#ifndef _FUNCTII_
#define _FUNCTII_
  typedef int (*TF1)(void *);
  typedef int (*TF2)(const void *, const void *);
#endif

#ifndef COADA
#define COADA

void* InitQ(size_t d,...); /* creeaza coada vida cu elemente de dimensiune d;
							  anumite implementari pot necesita si alti parametri */
int IntrQ(void* a, void* ae); /* adauga element la sfarsitul cozii */
int ExtrQ(void* a, void* ae); /* extrage primul element din coada la adresa ae */
int PrimQ(void* a, void* ae); /* copiaza primul element din coada la adresa ae */
int VidaQ(void* a);           /* test coada vida */
size_t NrEQ(void* a);         /* numarul de elemente din coada */
size_t DimEQ(void* a);        /* dimensiunea elementelor din coada */
void ResetQ(void* a);         /* transforma coada in coada vida */
void DistrQ(void** aa);       /* elibereaza intregul spatiu ocupat de coada */
size_t PrelQ(void* a, TF1 f); /* prelucreaza elementele, folosind functia f;
								   rezultat = numarul de elemente pentru care f != 0 */

/* Operatii de deplasare in coada destinatie din coada sursa.
   Rezultat < 0 in caz de eroare (de exemplu dimensiunile elementelor difera),
   0 daca sursa vida, 1 = succes */
int MutaQ(void *ad, void *as);  	/* muta in coada dest. primul element din sursa */
int ConcatQ(void *ad, void *as);	/* concatenare (muta toate elem.din sursa) */

#endif

