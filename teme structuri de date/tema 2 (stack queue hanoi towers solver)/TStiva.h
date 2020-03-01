/* VOICU Alex-Georgian */

/*-- TStiva.h --- tipul stiva ---*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#ifndef _FUNCTII_
#define _FUNCTII_
  typedef int (* TF1)(void *,...);
  typedef int (* TF2)(const void *, const void *);
#endif

#ifndef STIVA
#define STIVA

void* InitS(size_t d,...);    /* initializeaza stiva cu elem.de dimensiune d;
				  					anumite implementari pot necesita si alti parametri */
int Push(void* a, void* ae);  /* pune element in varful stivei */
int Pop (void* a, void* ae);  /* extrage elementul din varful stivei la adresa ae */
int Top (void* a, void* ae);  /* copiaza elementul din varful stivei la adresa ae */
int VidaS(void* a);           /* test stiva vida */
size_t NrES (void* a);        /* numarul de elemente din stiva */
size_t DimES(void* a);        /* dimensiunea elementelor din stiva */
void ResetS (void* a);        /* transforma stiva in stiva vida */
void DistrS (void** aa);      /* elibereaza intregul spatiu ocupat de stiva */
int InvS(void* a);            /* inverseaza continutul stivei */

/*- prelucrarea tuturor elementelor din stiva;
    -> numar elemente pentru care f != 0 -*/
size_t Prel_V_B(void* a, TF1 f); /* prelucreaza de la Varf spre Baza */
size_t Prel_B_V(void* a, TF1 f); /* prelucreaza de la Baza spre Varf */

/* Operatii de deplasare in stiva destinatie din stiva sursa.
   Rezultat < 0 in caz de eroare (de exemplu dimensiunile elementelor difera),
   0 daca sursa vida, 1 = succes */
int MutaS(void* ad, void* as);     /* muta elementul din varful stivei sursa */
int Suprapune(void* ad, void* as); /* muta stiva sursa peste destinatie */
int Rastoarna(void* ad, void* as); /* suprapune inversul sursei peste destinatie */

#endif

