//VOICU Alex-Georgian

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "info.h"

typedef int (*TFElem)(void*);     /* functie prelucrare element */
typedef int (*TFCmp)(void*, void*); /* functie de comparare doua elemente */
typedef void (*TF)(void*);     /* functie afisare/eliberare un element */

int comp_clasament(void *a, void *b)
{
    int rez;
    rez = ((Jucator)b)->nrMaini - ((Jucator)a)->nrMaini;
    if(rez == 0) return strcmp(((Jucator)b)->nume, ((Jucator)a)->nume);
    return rez;
}

void InsOrdLG(ALista aL, void* ae, TFCmp fcmp) // inserare ordonata in lista
{
    TLista aux = malloc(sizeof(Celula));
    if(!aux) return;

    aux->info = ae;

    if(*aL == NULL)
    {
        aux->urm = NULL;
        *aL = aux;
    }
    else if(fcmp(ae, ((Jucator)(*aL)->info)) < 0)
    {
        aux->urm = *aL;
        *aL = aux;
    }
    else
    {
        TLista p = *aL;
        while(p->urm != NULL)
        {
            if(fcmp(ae, ((Jucator)p->urm->info)) < 0) break;
            p = p->urm;
        }

        if(p->urm == NULL)
        {
            p->urm = aux;
            aux->urm = NULL;
        }
        else
        {
            aux->urm = p->urm;
            p->urm = aux;
        }
    }
}

void Distruge(ALista aL, TF free_elem) //distruge liste cu santinela dar si fara santinela
{
    TLista inc = *aL;
    TLista aux = *aL;     /* adresa celulei eliminate */
    if(*aL != NULL)
    {
        if (!aux) return; /* lista vida */
        if(aux->info) free_elem(aux->info);  /* elib.spatiul ocupat de element*/
        *aL = aux->urm;    /* deconecteaza celula din lista */
        free(aux);   /* elibereaza spatiul ocupat de celula */
    }

    while(*aL != NULL && inc != *aL)
    {
        aux = *aL;     /* adresa celulei eliminate */
        if (!aux) return; /* lista vida */
        if(aux->info) free_elem(aux->info);  /* elib.spatiul ocupat de element*/
        *aL = aux->urm;    /* deconecteaza celula din lista */
        free(aux);   /* elibereaza spatiul ocupat de celula */
    }
}

void eliminareJucator(void *player)
{
    free(((Jucator)player)->nume);
    free(player);
}

Jucator creareJucator(FILE *in)
{
    Jucator player;
    player = (Jucator)calloc(1, sizeof(struct jucator));
    if(player == NULL) return NULL;

    char s[100];
    fscanf(in,"%s%d", s, &player->nrMaini);
    if(player->nrMaini <= 0) {free(player); return NULL;}
    player->nume = malloc(strlen(s) + 1);
    if(player->nume == NULL) {free(player); return NULL;}
    memcpy(player->nume, s, strlen(s) + 1);

    return player;
}

void eliminareMasa(void *table)
{
    free(((Masa)table)->numeMasa);
    Distruge(&((Masa)table)->jucatori, eliminareJucator);
    free(table);
}

Masa creareMasa(FILE *in)
{
    Masa table;
    table = (Masa)calloc(1, sizeof(struct masa));
    if(table == NULL) return NULL;

    char s[100];
    fscanf(in,"%s%d%d", s, &table->nrCrtJucatori, &table->nrMaxJucatori);
    if(table->nrCrtJucatori <= 0 || table->nrMaxJucatori <= 0) {free(table); return NULL;}
    table->numeMasa = malloc(strlen(s) + 1);
    if(table->numeMasa == NULL) {free(table); return NULL;}
    memcpy(table->numeMasa, s, strlen(s) + 1);

    int i;
    table->jucatori = calloc(1, sizeof(Celula));
    if(table->jucatori == NULL) {eliminareMasa(table); return NULL;}

    TLista p = table->jucatori;
    for(i = 0; i < table->nrCrtJucatori; i++)
    {
        p->urm = calloc(1, sizeof(Celula));
        if(p->urm == NULL) {eliminareMasa(table); return NULL;}
        p = p->urm;
        p->info = creareJucator(in);
        if(p->info == NULL) {eliminareMasa(table); return NULL;}
    }
    p->urm = table->jucatori;
    return table;
}

void eliminareSala(void *room)
{
    Distruge(&((Sala)room)->masa, eliminareMasa);
    free(room);
}

Sala creareSala(char conf[])
{
    Sala room;
    room = calloc(1, sizeof(struct sala));
    if(room == NULL) return NULL;

    FILE *in;
    in = fopen(conf, "r");
    if(in == NULL) {free(room); return NULL;}

    fscanf(in, "%d", &room->nrMese);
    if(room->nrMese <= 0) {free(room); return NULL;}

    int i;
    room->masa = calloc(1, sizeof(Celula));
    if(room->masa == NULL) {eliminareSala(room); return NULL;}
    room->masa->info = creareMasa(in);
    if(room->masa->info == NULL) {eliminareSala(room); return NULL;}

    room->nrLocCrt += ((Masa)(room->masa->info))->nrCrtJucatori;
    room->nrLocMax += ((Masa)(room->masa->info))->nrMaxJucatori;

    TLista p = room->masa;
    for(i = 1; i < room->nrMese; i++)
    {
        p->urm = calloc(1, sizeof(Celula));
        if(p->urm == NULL) {eliminareSala(room); return NULL;}
        p = p->urm;
        p->info = creareMasa(in);
        if(p->info == NULL) {eliminareSala(room); return NULL;}

        room->nrLocCrt += ((Masa)(p->info))->nrCrtJucatori;
        room->nrLocMax += ((Masa)(p->info))->nrMaxJucatori;
    }

    fclose(in);
    return room;
}

void print(Sala room, FILE *out)
{
    TLista p = room->masa, q;
    if(room->nrMese == 0) fprintf(out, "Sala este inchisa!\n");
    while(p)
    {
        fprintf(out, "%s: ",((Masa)(p->info))->numeMasa);
        q = ((Masa)(p->info))->jucatori->urm;

        while(q->urm != ((Masa)(p->info))->jucatori)
        {
            fprintf(out, "%s - %d; ", ((Jucator)(q->info))->nume, ((Jucator)(q->info))->nrMaini);
            q = q->urm;
        }

        fprintf(out, "%s - %d.\n", ((Jucator)(q->info))->nume, ((Jucator)(q->info))->nrMaini);

        p = p->urm;
    }
}

void noroc(Sala room, FILE *in, FILE *out)
{
    char nume_masa[100], nume_jucator[100];
    int grad_noroc;
    fscanf(in, "%s%s%d", nume_masa, nume_jucator, &grad_noroc);
    TLista p = room->masa, q = NULL, inc = NULL;
    while(p)
    {
        if(strcmp( ((Masa)(p->info))->numeMasa, nume_masa) == 0)
        {
            inc = ((Masa)(p->info))->jucatori;
            q = inc->urm;
            while(q != inc)
            {
                if(strcmp( ((Jucator)(q->info))->nume, nume_jucator) == 0)
                {
                    ((Jucator)(q->info))->nrMaini += grad_noroc;
                    break;
                }
                q = q->urm;
            }
            break;
        }
        p = p->urm;
    }
    if(p == NULL) fprintf(out, "Masa %s nu exista!\n", nume_masa);
    if(p != NULL && q == inc) fprintf(out, "Jucatorul %s nu exista la masa %s!\n", nume_jucator, nume_masa);
}

void scoate_masa(Sala room, ALista table) //inchide o masa fara jucatori
{
    if(*table == room->masa)
        room->masa = room->masa->urm;
    else
    {
        TLista p = room->masa;
        while(p->urm != NULL)
        {
            if(p->urm == *table) break;
            p = p->urm;
        }
        p->urm = (*table)->urm;
    }
    eliminareMasa((*table)->info);
    free(*table);
    *table = NULL;
}

void scoate_jucator(TLista anterior)
{
    TLista aux = anterior->urm;
    anterior->urm = aux->urm;
    eliminareJucator(aux->info);
    free(aux);
}

void ghinion(Sala room, FILE *in, FILE *out)
{
    char nume_masa[100], nume_jucator[100];
    int grad_ghinion;
    fscanf(in, "%s%s%d", nume_masa, nume_jucator, &grad_ghinion);
    TLista p = room->masa, q = NULL, inc = NULL;
    while(p)
    {
        if(strcmp( ((Masa)(p->info))->numeMasa, nume_masa) == 0)
        {
            inc = ((Masa)(p->info))->jucatori;
            q = inc;
            while(q->urm != inc)
            {
                if(strcmp( ((Jucator)(q->urm->info))->nume, nume_jucator) == 0)
                {
                    ((Jucator)(q->urm->info))->nrMaini -= grad_ghinion;
                    break;
                }
                q = q->urm;
            }
            break;
        }
        p = p->urm;
    }
    if(p == NULL) {fprintf(out, "Masa %s nu exista!\n", nume_masa);return;}
    if(p != NULL && q->urm == inc) {fprintf(out, "Jucatorul %s nu exista la masa %s!\n", nume_jucator, nume_masa);return;}

    if(((Jucator)(q->urm->info))->nrMaini <= 0)
    {
        scoate_jucator(q);
        ((Masa)(p->info))->nrCrtJucatori--;
        room->nrLocCrt--;
        if(((Masa)(p->info))->nrCrtJucatori == 0)
        {

            room->nrLocMax -= ((Masa)(p->info))->nrMaxJucatori;
            room->nrMese--;
            scoate_masa(room, &p);
        }
    }
}

void tura(Sala room, FILE *in, FILE *out)
{
    char nume_masa[100];
    fscanf(in, "%s", nume_masa);
    TLista p = room->masa, q = NULL, inc = NULL;
    while(p)
    {
        if(strcmp( ((Masa)(p->info))->numeMasa, nume_masa) == 0) break;
        p = p->urm;
    }
    if(p == NULL) {fprintf(out, "Masa %s nu exista!\n", nume_masa);return;}

    inc = ((Masa)(p->info))->jucatori; //rotatie
    q = inc;
    while(q->urm != inc)
        q = q->urm;
    q->urm = inc->urm;
    inc->urm = inc->urm->urm;
    q->urm->urm = inc;

    inc = ((Masa)(p->info))->jucatori;
    q = inc;
    while(q->urm != inc)
    {
        ((Jucator)(q->urm->info))->nrMaini -= 1;
        if(((Jucator)(q->urm->info))->nrMaini <= 0)
        {
            scoate_jucator(q);
            ((Masa)(p->info))->nrCrtJucatori--;
            room->nrLocCrt--;
            if(((Masa)(p->info))->nrCrtJucatori == 0)
            {
                room->nrLocMax -= ((Masa)(p->info))->nrMaxJucatori;
                room->nrMese--;
                scoate_masa(room, &p);
                break;
            }
        }
        else q = q->urm;
    }
}

void tura_completa(Sala room)
{
    TLista p = room->masa, q = NULL, inc = NULL, urm;
    while(p)
    {
        urm = p->urm;

        inc = ((Masa)(p->info))->jucatori; //rotatie
        q = inc;
        while(q->urm != inc)
            q = q->urm;
        q->urm = inc->urm;
        inc->urm = inc->urm->urm;
        q->urm->urm = inc;

        inc = ((Masa)(p->info))->jucatori;
        q = inc;
        while(q->urm != inc)
        {
            ((Jucator)(q->urm->info))->nrMaini -= 1;
            if(((Jucator)(q->urm->info))->nrMaini <= 0)
            {
                scoate_jucator(q);
                ((Masa)(p->info))->nrCrtJucatori--;
                room->nrLocCrt--;
                if(((Masa)(p->info))->nrCrtJucatori == 0)
                {
                    room->nrLocMax -= ((Masa)(p->info))->nrMaxJucatori;
                    room->nrMese--;
                    scoate_masa(room, &p);
                    break;
                }
            }
            else q = q->urm;
        }
        p = urm;
    }
}

void inchide(Sala room, FILE *in, FILE *out)
{
    char nume_masa[100];
    TLista table = NULL;
    fscanf(in, "%s", nume_masa);
    TLista p = room->masa, q = NULL, inc = NULL;

    if(p != NULL && strcmp(nume_masa, ((Masa)(p->info))->numeMasa) == 0)
    {
        if(0 >=
           room->nrLocMax - room->nrLocCrt - ((Masa)(p->info))->nrMaxJucatori)
        {fprintf(out, "Nu exista suficiente locuri in sala!\n");return;}

        room->masa = room->masa->urm;
        table = p;
    }
    else
    {
        TLista p = room->masa;
        while(p->urm != NULL)
        {
            if(strcmp(nume_masa, ((Masa)(p->urm->info))->numeMasa) == 0)
            {
                table = p->urm;
                break;
            }
            p = p->urm;
        }

        if(table != NULL && 0 >=
           room->nrLocMax - room->nrLocCrt - ((Masa)(table->info))->nrMaxJucatori)
        {fprintf(out, "Nu exista suficiente locuri in sala!\n");return;}

        if(table) p->urm = table->urm;
    }

    if(table == NULL) {fprintf(out, "Masa %s nu exista!\n", nume_masa);return;}

    room->nrMese--;
    room->nrLocMax -= ((Masa)(table->info))->nrMaxJucatori;

    p = ((Masa)(table->info))->jucatori;
    TLista u = p->urm; //ultimul jucator ramas de scos din masa de inchis
    p->urm = NULL; //in lista ramane doar santinela
    p = room->masa;
    while(p != NULL && ((Masa)(table->info))->nrCrtJucatori > 0)
    {
        q = ((Masa)(p->info))->jucatori;
        inc = q;
        while(q->urm != inc) //se cauta sfarsitul listei la care vom adauga
        {
            q = q->urm;
        }
        for(; ((Masa)(p->info))->nrCrtJucatori < ((Masa)(p->info))->nrMaxJucatori
            && ((Masa)(table->info))->nrCrtJucatori > 0;
            ((Masa)(p->info))->nrCrtJucatori++) //aici se adauga cat timp se poate
        {
            q->urm = u;
            u = u->urm;
            q->urm->urm = inc;
            q = q->urm;
            ((Masa)(table->info))->nrCrtJucatori--;
        }
        p = p->urm;
    }

    eliminareMasa(table->info);
    free(table);
    table = NULL;
}

void clasament(Sala room, FILE *in, FILE *out)
{
    char nume_masa[100];
    fscanf(in, "%s", nume_masa);
    TLista p = room->masa, q = NULL, inc = NULL;

    while(p)
    {
        if(strcmp( ((Masa)(p->info))->numeMasa, nume_masa) == 0) break;
        p = p->urm;
    }
    if(p == NULL) {fprintf(out, "Masa %s nu exista!\n", nume_masa);return;}

    TLista lista_ord = NULL;
    Jucator j;
    inc = ((Masa)(p->info))->jucatori;
    q = inc->urm;
    while(q != inc)
    {
        j = malloc(sizeof(struct jucator));
        memcpy(j, q->info, sizeof(struct jucator));
        j->nume = strdup(((Jucator)(q->info))->nume);
        InsOrdLG(&lista_ord, j, comp_clasament);
        q = q->urm;
    }

    //afisare

    fprintf(out, "Clasament %s:\n", nume_masa);
    p = lista_ord;
    while(p != NULL)
    {
        fprintf(out, "%s %d\n", ((Jucator)(p->info))->nume, ((Jucator)(p->info))->nrMaini);
        p = p->urm;
    }

    Distruge(&lista_ord, eliminareJucator);
}

int main(int argc, char **argv)
{
    //if(argc != 4) {printf("Numar insuficient de parametri.\n"); return 1;}
    Sala room = NULL;
    room = creareSala(argv[1]);

    FILE *in, *out;
    in = fopen(argv[2], "r");
    out = fopen(argv[3], "w");

    char s[100];
    while(fscanf(in, "%s", s) == 1)
    {
        if(strcmp(s, "print") == 0)
        {
            print(room, out);
        }
        else if(strcmp(s, "noroc") == 0)
        {
            noroc(room, in, out);
        }
        else if(strcmp(s, "ghinion") == 0)
        {
            ghinion(room, in, out);
        }
        else if(strcmp(s, "tura") == 0)
        {
            tura(room, in, out);
        }
        else if(strcmp(s, "tura_completa") == 0)
        {
            tura_completa(room);
        }
        else if(strcmp(s, "inchide") == 0)
        {
            inchide(room, in, out);
        }
        else if(strcmp(s, "clasament") == 0)
        {
            clasament(room, in, out);
        }
    }

    eliminareSala(room);
    fclose(in);
    fclose(out);
    return 0;
}
