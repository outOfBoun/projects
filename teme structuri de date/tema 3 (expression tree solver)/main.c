/* VOICU Alex-Georgian */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "tlg.h"

typedef struct
{
    char cod_op; //0 = valoare numerica. 1-6 = operatii
    int val; //valabil doar daca cod_operatie = 0
    char *nume; ////valabil doar daca cod_operatie = -1
}Telex; //element de expresie (operand sau operator)

typedef struct
{
    char *nume;
    int val;
}Tvar;

typedef struct nod {
	Telex info;
	struct nod *st, *dr;
} TNod, *TArb, **AArb;

void elimina_Tvar(void *a)
{
    free(((Tvar*)a)->nume);
    free(a);
}

void distrugeArb(TArb r)
{
	if (!r) return;
	distrugeArb(r->st);
	distrugeArb(r->dr);
	if(r->info.nume != NULL)
        free(r->info.nume);
	free (r);
}

int CitireVar(FILE *fin, ALG rlista)
{
    int n, i, vaux;
    char nume[15];
    Tvar *info;
    fscanf(fin, "%d", &n);
    for(i = 0; i < n; i++)
    {
        fscanf(fin, "%s = %d", nume, &vaux);
        info = malloc(sizeof(Tvar));
        if(!info)
        {
            Distruge(rlista, elimina_Tvar);
            return 1;
        }

        info->nume = malloc(strlen(nume)+1);
        if(!info->nume)
        {
            free(info);
            Distruge(rlista, elimina_Tvar);
            return 1;
        }

        strcpy(info->nume, nume);
        info->val = vaux;

        InsLG(rlista, info);
    }
    return 0;
}

void empty_line(FILE *fin)
{
    char str[20], delim = 0;
    do
    {
        fscanf(fin, "%s%c", str, &delim);
    }
    while(delim != '\n' && delim != 0);
}

void scanare_op(FILE *fin, char *str, char *delim)
{
    char aux;
    fscanf(fin, "%s", str);
    aux = fgetc(fin);
    *delim = aux;
    while(!feof(fin) && (aux == ' ' || aux == '\n'))
    {
        *delim = aux;
        aux = fgetc(fin);
    }
    if(aux != ' ' || aux != '\n') ungetc(aux, fin);
}

int ConstruireExpr(FILE *fin, FILE *fout, TLG lista_var, AArb rarb)
{
    char str[20], delim = 0;
    int err;

    scanare_op(fin, str, &delim);

    *rarb = calloc(1, sizeof(TNod));
	if (!*rarb)
    {
        fprintf(fout, "Nu s-a putut aloca memorie\n");
        if(delim == '\n' || delim == 0 || delim == -1) return 1;
        else empty_line(fin);
        return 2;
    }

    if(str[1]==0 && strchr("+-*/^", str[0]))
    {
        if(delim == '\n' || delim == 0 || delim == -1)
        {
            fprintf(fout, "Sintaxa invalida!\n");

            return 1;
        }

        if(str[0] == '+')
            (*rarb)->info.cod_op = 1;
        else if(str[0] == '-')
            (*rarb)->info.cod_op = 2;
        else if(str[0] == '*')
            (*rarb)->info.cod_op = 3;
        else if(str[0] == '/')
            (*rarb)->info.cod_op = 4;
        else if(str[0] == '^')
            (*rarb)->info.cod_op = 5;

        err = ConstruireExpr(fin, fout, lista_var, &((*rarb)->st));
        if(err == -1)
        {
            fprintf(fout, "Sintaxa invalida!\n");
            return 1;
        }
        else if(err > 0)
            return err;

        err = ConstruireExpr(fin, fout, lista_var, &((*rarb)->dr));
        return err;
    }
    else if(strcmp("sqrt", str) == 0)
    {
        if(delim == '\n' || delim == 0 || delim == -1)
        {
            fprintf(fout, "Sintaxa invalida!\n");
            return 1;
        }

        (*rarb)->info.cod_op = 6;

        err = ConstruireExpr(fin, fout, lista_var, &((*rarb)->st));

        return err;
    }
    else if((str[0] >= '0' && str[0] <= '9') || (str[1] >= '0' && str[1] <= '9')) //numar
    {
        (*rarb)->info.val = atoi(str);
        (*rarb)->info.cod_op = 0;
    }
    else //variabila
    {
        (*rarb)->info.cod_op = -1;
        (*rarb)->info.nume = malloc(strlen(str)+1);
        if(!(*rarb)->info.nume)
        {
            fprintf(fout, "Nu s-a putut aloca memorie\n");
            if(delim == ' ') empty_line(fin);
            return 1;
        }
        strcpy((*rarb)->info.nume, str);
    }

    if(delim == '\n' || delim == 0 || delim == -1) return -1; // sfarsitul liniei de citit
    return 0;
}

int RezolvareExpr(TArb arb, int *err, FILE *fout, TLG lista_var)
{
    double a,b;
    if(arb->info.cod_op == 0) return arb->info.val;
    else if(arb->info.cod_op == -1)
    {
        TLG p = lista_var;
        while(p != NULL)
        {
            if(strcmp(((Tvar*)(p->info))->nume, arb->info.nume) == 0 )
                break;
            p = p->urm;
        }
        if(p == NULL)
        {
            fprintf(fout, "Variable %s undeclared\n", arb->info.nume);
            *err = 1; //nu a fost gasita variabila
            return 0;
        }
        return ((Tvar*)(p->info))->val;
    }

    a = RezolvareExpr(arb->st, err, fout, lista_var);
    if(*err == 1) return 0;
    if(arb->info.cod_op != 6)
    {
        b = RezolvareExpr(arb->dr, err, fout, lista_var);
        if(*err == 1) return 0;
    }

    if(arb->info.cod_op == 1)
        return a + b;
    else if(arb->info.cod_op == 2)
        return a - b;
    else if(arb->info.cod_op == 3)
        return a * b;
    else if(arb->info.cod_op == 4)
    {
        if(b == 0)
        {
            *err = 1;
            fprintf(fout, "Invalid expression\n");
            return 0;
        }
        return a / b;
    }
    else if(arb->info.cod_op == 5)
        return pow(a,b);
    else if(arb->info.cod_op == 6)
    {
        if(a < 0)
        {
            *err = 1;
            fprintf(fout, "Invalid expression\n");
            return 0;
        }
        return sqrt(a);
    }
}

int main(int argc,char *argv[])
{
    FILE *fin, *fout;
    if(argc != 3) return 1;
    fin = fopen(argv[1], "r");
    if(fin == NULL) return 1;
    fout = fopen(argv[2], "w");
    if(fout == NULL) return 1;

    TLG lista_var = 0;
    if(CitireVar(fin, &lista_var) == 1)
    {
        fclose(fin);
        return 1;
    }

    int m, i, err;
    int sol;
    TArb arb;
    fscanf(fin, "%d", &m);
    for(i = 0; i < m; i++)
    {
        arb = NULL;
        err = ConstruireExpr(fin, fout, lista_var, &arb);
        if(err >= 0)
        {
            if(err == 0)
            {
                fprintf(fout, "Sintaxa invalida!\n");
                empty_line(fin);
            }
            distrugeArb(arb);
            continue;
        }
        //rezolva expr
        err = 0;
        sol = RezolvareExpr(arb, &err, fout, lista_var);
        if(err == 0)
            fprintf(fout, "%d\n", sol);
        distrugeArb(arb);
    }

    Distruge(&lista_var, elimina_Tvar);
    fclose(fin);
    fclose(fout);

    return 0;
}
