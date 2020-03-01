/* VOICU Alex-Georgian */

/*--- functiiLG.c -- operatii de baza pentru lista simplu inlantuita generica---*/
#include "tlg.h"

int InsLG(ALG aL, void* ae)
{
  TLG aux = malloc(sizeof(TCelulaG));
  if(!aux) return 0;
  aux->info = ae;
  aux->urm = *aL;
  *aL = aux;
  return 1;
}

int Ins_sfLG(ALG aL, void* ae) //inserare la sfarsit de lista
{
  TLG aux = malloc(sizeof(TCelulaG));
  if(!aux) return 0;
  aux->info = ae;
  aux->urm = 0;

  if(! *aL)
  {
      *aL = aux;
  }
  else
  {
      TLG p = *aL;
      while(p->urm != NULL)
      {
          p = p->urm;
      }
      p->urm = aux;
  }

  return 1;
}

void Distruge(ALG aL, TF free_elem) /* distruge lista */
{
  while(*aL != NULL)
    {
        TLG aux = *aL;     /* adresa celulei eliminate */
        if (!aux) return; /* lista vida */
        free_elem(aux->info);  /* elib.spatiul ocupat de element*/
        *aL = aux->urm;    /* deconecteaza celula din lista */
        free(aux);   /* elibereaza spatiul ocupat de celula */
    }
}

size_t LungimeLG(ALG a)      /* numarul de elemente din lista */
{
  size_t lg = 0;
  TLG p = *a;
  for (; p != NULL; p = p->urm) lg++;  /* parcurge lista, numarand celulele */
  return lg;
}

void Afisare(ALG aL, TF afiEL)
{
  if(!*aL) { printf("Lista vida\n"); return;}
  printf("Lista generica: \n");
  for(; *aL; aL = &(*aL)->urm)
    afiEL((*aL)->info);
  printf("\n");
}


