/* VOICU Alex-Georgian */

#include "tlg.h"

typedef struct
{
    size_t dime;
    TLG baza, vf; //prima si ultima celula din lista
}TStivaL, *ASL;

void* InitS(size_t d,...)
{
    ASL a = (ASL)calloc(1, sizeof(TStivaL));
    if(!a) return NULL;
    a->dime = d;
    return (void*)a;
}

void DistrS(void** aa)
{
    ASL a = *aa;
    Distruge(&(a->baza), free);
    free(a);
    *aa = NULL;
}

int Push(void* a, void* ae)
{
    void *elem;
    elem = malloc(((ASL)a)->dime);
    if(!elem)
    {
        return 0;
    }
    memcpy(elem, ae, ((ASL)a)->dime);

    if(((ASL)a)->baza == NULL)
    {
        Ins_sfLG(&(((ASL)a)->baza), elem);
        ((ASL)a)->vf = ((ASL)a)->baza;
    }
    else
    {
        Ins_sfLG(&(((ASL)a)->vf), elem);
        ((ASL)a)->vf = ((ASL)a)->vf->urm;
    }
    return 1;
}

int Pop(void* a, void* ae)
{
    if(! ((ASL)a)->vf) return 0;
    TLG aux = ((ASL)a)->vf;
    memcpy(ae, ((ASL)a)->vf->info, ((ASL)a)->dime);

    ((ASL)a)->vf = ((ASL)a)->baza;
    if(((ASL)a)->vf->urm == NULL)
    {
        ((ASL)a)->vf = NULL;
        ((ASL)a)->baza = NULL;
    }
    else
    {
        while(((ASL)a)->vf->urm != aux)
        {
            ((ASL)a)->vf = ((ASL)a)->vf->urm;
        }
        ((ASL)a)->vf->urm = NULL;
    }

    free(aux->info);
    free(aux);
    return 1;
}

int VidaS(void* a)
{
    if(((ASL)a)->vf == NULL) return 1;
    return 0;
}
