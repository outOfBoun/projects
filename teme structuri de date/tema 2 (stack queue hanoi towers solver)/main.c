/* VOICU Alex-Georgian */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TCoada.h"
#include "TStiva.h"
#include "tlg.h"

typedef struct //structura unui sistem hanoi
{
    char *culoare, executat;
    int nr_disc;
    void *s_a, *s_b, *s_c, *c_mutari;
}sistem_h;

typedef struct //structura unei mutari din cadrul unui sistem hanoi
{
    char s, d;
}mutare_h;

void creare_sistem_h(ALG rlista_sisteme, char *culoare)
{
    //alocari si initializari
    sistem_h *psh = malloc(sizeof(sistem_h));
    if(!psh) return;
    psh->culoare = malloc(strlen(culoare) + 1);
    if(!psh->culoare) {free(psh) ;return;}
    strcpy(psh->culoare, culoare);
    psh->nr_disc = 0;
    psh->executat = 0;
    psh->s_a = InitS(sizeof(int));
    psh->s_b = InitS(sizeof(int));
    psh->s_c = InitS(sizeof(int));
    psh->c_mutari = InitQ(sizeof(mutare_h));

    //introducerea sistemului in lista de sisteme
    InsLG(rlista_sisteme, psh);
}

void add(ALG rlista_sisteme, FILE *fin)
{
    char culoare[128];
    int dim_disc;
    fscanf(fin, "%s %d", culoare, &dim_disc);
    TLG p = *rlista_sisteme;
    while(p != NULL) //cautam in lista de sisteme culoarea citita
    {
        if(strcmp( ((sistem_h*)(p->info))->culoare , culoare) == 0) break;
        p = p->urm;
    }

    if(p == NULL) //daca sistemul nu exista deja mai intai il facem
    {
        creare_sistem_h(rlista_sisteme, culoare);
        p = *rlista_sisteme;
    }

    //adaugarea discului in sistemul gasit sau creat

    ((sistem_h*)(p->info))->nr_disc++;

    //daca stiva A este vida atunci inseram direct primul disc
    if(VidaS(((sistem_h*)(p->info))->s_a))
    {
        Push(((sistem_h*)(p->info))->s_a, &dim_disc);
    }
    else //altfel inseram ordonat
    {
        void *saux = InitS(sizeof(int));
        int disc_scos;

        do //scoaterea discurilor mai mici in stiva auxiliara
        {
            Pop(((sistem_h*)(p->info))->s_a, &disc_scos);
            if(disc_scos < dim_disc)
                Push(saux, &disc_scos);
            else
            {
                Push(((sistem_h*)(p->info))->s_a, &disc_scos);
                break;
            }
        }
        while(!VidaS(((sistem_h*)(p->info))->s_a));

        //introducerea discului citit
        Push(((sistem_h*)(p->info))->s_a, &dim_disc);

        while(!VidaS(saux)) //intoarcerea discurilor din saux in stiva A
        {
            Pop(saux, &disc_scos);
            Push(((sistem_h*)(p->info))->s_a, &disc_scos);
        }
        DistrS(&saux);
    }
}

void elib_sistem_h(void *p) //functia de dealocare a unei structuri de sistem hanoi
{
    DistrS(&(((sistem_h*)p)->s_a));
    DistrS(&(((sistem_h*)p)->s_b));
    DistrS(&(((sistem_h*)p)->s_c));
    DistrQ(&(((sistem_h*)p)->c_mutari));
    free(((sistem_h*)p)->culoare);
    free(p);
}

void afisare_stiva_bv(void *s, FILE *fout) //functia de afisare a unei tije
{
    void *saux = InitS(sizeof(int));
    int disc_scos;
    while(!VidaS(s)) //mutarea discurilor in stiva auxiliara
    {
        Pop(s, &disc_scos);
        Push(saux, &disc_scos);
    }
    while(!VidaS(saux)) //mutarea inapoi in stiva initiala odata cu afisarea lor
    {
        Pop(saux, &disc_scos);
        fprintf(fout, " %d", disc_scos);
        Push(s, &disc_scos);
    }
    fprintf(fout, "\n");
    DistrS(&saux);
}

void show(ALG rlista_sisteme, FILE *fin, FILE *fout) //show poate crea un sistem care nu exista
{
    char culoare[128];
    fscanf(fin, "%s", culoare);
    TLG p = *rlista_sisteme;
    while(p != NULL) //cautam in lista de sisteme culoarea citita
    {
        if(strcmp( ((sistem_h*)(p->info))->culoare , culoare) == 0) break;
        p = p->urm;
    }
    if(p == NULL) //daca sistemul nu exista deja mai intai il facem
    {
        creare_sistem_h(rlista_sisteme, culoare);
        p = *rlista_sisteme;
    }

    fprintf(fout, "A_%s:", ((sistem_h*)(p->info))->culoare);
    afisare_stiva_bv(((sistem_h*)(p->info))->s_a, fout);
    fprintf(fout, "B_%s:", ((sistem_h*)(p->info))->culoare);
    afisare_stiva_bv(((sistem_h*)(p->info))->s_b, fout);
    fprintf(fout, "C_%s:", ((sistem_h*)(p->info))->culoare);
    afisare_stiva_bv(((sistem_h*)(p->info))->s_c, fout);
}

void hanoi(void *coada_mutari, int n, char t_init, char t_aux, char t_fin)
{ //algoritmul recursiv pentru hanoi
    if(n == 0) return;
    hanoi(coada_mutari, n-1, t_init, t_fin, t_aux);
    mutare_h m;
    m.s = t_init;
    m.d = t_fin;
    IntrQ(coada_mutari, &m);
    hanoi(coada_mutari, n-1, t_aux, t_init, t_fin);
}

void play(TLG lista_sisteme, FILE *fin, FILE *fout)
{
    char culoare[128];
    mutare_h m;
    int nr_mut, disc;
    fscanf(fin, "%s %d", culoare, &nr_mut);
    TLG p = lista_sisteme;
    while(p != NULL) //cautam in lista de sisteme culoarea citita
    {
        if(strcmp( ((sistem_h*)(p->info))->culoare , culoare) == 0) break;
        p = p->urm;
    }
    if(p == NULL) return;

    //se simuleaza hanoi pe sistemul respectiv daca nu s-a facut pana acum
    if(((sistem_h*)(p->info))->executat == 0)
    {
        hanoi(((sistem_h*)(p->info))->c_mutari, ((sistem_h*)(p->info))->nr_disc,
              'A', 'B', 'C');
        ((sistem_h*)(p->info))->executat = 1;
    }

    //mutarea elementelor de pe tije conform cozii de mutari
    while(nr_mut > 0 && !VidaQ(((sistem_h*)(p->info))->c_mutari))
    {
        nr_mut--;
        ExtrQ(((sistem_h*)(p->info))->c_mutari, &m);

        if(m.s == 'A') Pop(((sistem_h*)(p->info))->s_a, &disc);
        else if(m.s == 'B') Pop(((sistem_h*)(p->info))->s_b, &disc);
        else if(m.s == 'C') Pop(((sistem_h*)(p->info))->s_c, &disc);

        if(m.d == 'A') Push(((sistem_h*)(p->info))->s_a, &disc);
        else if(m.d == 'B') Push(((sistem_h*)(p->info))->s_b, &disc);
        else if(m.d == 'C') Push(((sistem_h*)(p->info))->s_c, &disc);
    }
}

void show_moves(TLG lista_sisteme, FILE *fin, FILE *fout)
{
    char culoare[128];
    mutare_h m;
    int nr_mut;
    fscanf(fin, "%s %d", culoare, &nr_mut);
    TLG p = lista_sisteme;
    while(p != NULL) //cautam in lista de sisteme culoarea citita
    {
        if(strcmp( ((sistem_h*)(p->info))->culoare , culoare) == 0) break;
        p = p->urm;
    }
    if(p == NULL) return;

    //se simuleaza hanoi pe sistemul respectiv daca nu s-a facut pana acum
    if(((sistem_h*)(p->info))->executat == 0)
    {
        hanoi(((sistem_h*)(p->info))->c_mutari ,((sistem_h*)(p->info))->nr_disc,
              'A', 'B', 'C');
        ((sistem_h*)(p->info))->executat = 1;
    }

    //afisarea mutarilor cerute
    fprintf(fout, "M_%s:", culoare);
    void *caux = InitQ(sizeof(mutare_h));
    while(nr_mut > 0 && !VidaQ(((sistem_h*)(p->info))->c_mutari))
    {
        nr_mut--;
        ExtrQ(((sistem_h*)(p->info))->c_mutari, &m);
        IntrQ(caux, &m);
        fprintf(fout, " %c->%c", m.s, m.d);
    }
    fprintf(fout, "\n");
    //refacerea cozii de mutari
    ConcatQ(caux, ((sistem_h*)(p->info))->c_mutari);
    DistrQ(&(((sistem_h*)(p->info))->c_mutari));
    ((sistem_h*)(p->info))->c_mutari = caux;
}

int main(int argc, char **argv)
{
    if(argc != 3) return 1;
    FILE *fin, *fout;
    fin = fopen(argv[1], "r");
    fout = fopen(argv[2], "w");

    int n;
    fscanf(fin, "%d", &n);
    char s[128];

    TLG lista_sisteme = NULL;

    while(n)
    {
        n--;
        fscanf(fin, "%s", s);
        if(strcmp(s, "add") == 0)
        {
            add(&lista_sisteme, fin);
        }
        else if(strcmp(s, "show") == 0)
        {
            show(&lista_sisteme, fin, fout);
        }
        else if(strcmp(s, "play") == 0)
        {
            play(lista_sisteme, fin, fout);
        }
        else if(strcmp(s, "show_moves") == 0)
        {
            show_moves(lista_sisteme, fin, fout);
        }
        else
        {
            break;
        }
    }

    fclose(fin);
    fclose(fout);
    Distruge(&lista_sisteme, elib_sistem_h);

    return 0;
}
