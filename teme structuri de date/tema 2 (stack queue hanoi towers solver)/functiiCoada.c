/* VOICU Alex-Georgian */

#include "tlg.h"

typedef struct
{
    size_t dime;
    TLG p, u; //prima si ultima celula din lista
}TCoadaL, *AQL;

void* InitQ(size_t d,...)
{
    AQL a = (AQL)calloc(1, sizeof(TCoadaL));
    if(!a) return NULL;
    a->dime = d;
    return (void*)a;
}

void DistrQ(void** aa)
{
    AQL a = *aa;
    Distruge(&(a->p), free);
    free(a);
    *aa = NULL;
}

int IntrQ(void* a, void* ae)
{
    void *elem;
    elem = malloc(((AQL)a)->dime);
    if(!elem)
    {
        return 0;
    }
    memcpy(elem, ae, ((AQL)a)->dime);

    if(((AQL)a)->p == NULL)
    {
        Ins_sfLG(&(((AQL)a)->p), elem);
        ((AQL)a)->u = ((AQL)a)->p;
    }
    else
    {
        Ins_sfLG(&(((AQL)a)->u), elem);
        ((AQL)a)->u = ((AQL)a)->u->urm;
    }
    return 1;
}

int ExtrQ(void* a, void* ae)
{
    if(! ((AQL)a)->p) return 0;
    TLG aux = ((AQL)a)->p;
    memcpy(ae, ((AQL)a)->p->info, ((AQL)a)->dime);
    ((AQL)a)->p = ((AQL)a)->p->urm;
    free(aux->info);
    free(aux);
    return 1;
}

int VidaQ(void* a)
{
    if(((AQL)a)->p == NULL) return 1;
    return 0;
}

int ConcatQ(void *ad, void *as)
{
    if(VidaQ(ad))
    {
        ((AQL)ad)->p = ((AQL)as)->p;
        ((AQL)ad)->u = ((AQL)as)->u;
    }
    else
    {
        ((AQL)ad)->u->urm = ((AQL)as)->p;
        ((AQL)ad)->u = ((AQL)as)->u;
    }
    ((AQL)as)->p = NULL;
    ((AQL)as)->u = NULL;
    return 1;
}

