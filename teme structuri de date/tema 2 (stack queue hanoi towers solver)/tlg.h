/* VOICU Alex-Georgian  */

/*-- tlg.h --- LISTA SIMPLU INLANTUITA GENERICA ---*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#ifndef _LISTA_GENERICA_
#define _LISTA_GENERICA_

typedef struct celulag
{
  void* info;           /* adresa informatie */
  struct celulag *urm;   /* adresa urmatoarei celule */
} TCelulaG, *TLG, **ALG; /* tipurile Celula, Lista si Adresa_Lista */

typedef int (*TFElem)(void*);     /* functie prelucrare element */
typedef int (*TFCmp)(void*, void*); /* functie de comparare doua elemente */
typedef void (*TF)(void*);     /* functie afisare/eliberare un element */



/* functii lista generica */
int InsLG(ALG, void*);  /*- inserare la inceput reusita sau nu (1/0) -*/
void Distruge(ALG aL, TF fe); /* distruge lista */
size_t LungimeLG(ALG);   /* numarul de elemente din lista */
/* afiseaza elementele din lista, folosind o functie de tip TFAfi */
void Afisare(ALG, TF);
int Ins_sfLG(ALG, void*);

#endif

