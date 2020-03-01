// Voicu Alex-Georgian

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp_header.h"
#include "cifre_header.h"

#define IMSIZE 105

#pragma pack(1)
struct pixel
{
    unsigned char b, g, r;
};
#pragma pack()

struct simage
{
    struct bmp_fileheader bmpf;
    struct bmp_infoheader bmpi;
    struct pixel px[IMSIZE][IMSIZE];
};

struct scifra
{
    struct pixel color; //culoarea cifrei
    unsigned char ln, col; //pozitia coltului din stanga sus a cifrei in coordonatele imaginii
    unsigned char cif_type; //tipul cifrei
};

struct pixel_coord //pt bonus
{
    struct pixel color;
    unsigned char ln, col;
};

struct pixel_average //pt bonus
{
    unsigned int b, g, r, num;
};

FILE *in, *iimg, *oimg;

unsigned char image_map[IMSIZE + 2][IMSIZE + 2];
const unsigned char zerooo[]={0,0,0};

void read_image(struct simage *a)
{
    fread(&(a->bmpf), sizeof(struct bmp_fileheader), 1, iimg); //citim headerul
    fread(&(a->bmpi), sizeof(struct bmp_infoheader), 1, iimg);

    int i, j, padding, current_byte = 0;

    padding = (4 - (a->bmpi.width * 3) % 4) % 4; //calculam paddingul
    for(i = a->bmpi.height - 1; i >= 0; i--)
    {
        fread(a->px[i], sizeof(struct pixel), a->bmpi.width, iimg); //citim cate o linie
        fseek(iimg, padding, SEEK_CUR); //sarim peste padding
    }
}

void write_image(struct simage *a)
{
    fwrite(&(a->bmpf), sizeof(struct bmp_fileheader), 1, oimg); //scriem headerul
    fwrite(&(a->bmpi), sizeof(struct bmp_infoheader), 1, oimg);

    int i, j, padding, current_byte = 0;

    padding = (4 - (a->bmpi.width * 3) % 4) % 4; //calculam paddingul
    for(i = a->bmpi.height - 1; i >= 0; i--)
    {
        fwrite(a->px[i], sizeof(struct pixel), a->bmpi.width, oimg); //scriem cate o linie
        fwrite(&zerooo, sizeof(unsigned char), padding, oimg); //scriem paddingul
    }
}

int check_area(unsigned char ln, unsigned char col)
{
    char q[55] = {0,1,2,3,4,5,6,7,8,9}, p=0, u=9, cr, i, ucr;

    for(i = 0; i < 7 && p <= u; i++)
    {
        ucr = u;
        while(p <= ucr)
        {
            cr = q[p];
            p++;
            if(strncmp(image_map[ln + i] + col, cifre[cr][i], 7) == 0)
            {
                q[++u] = cr;
            }
        }
    }
    if(p == u) return q[p];
    return -1;
}

void draw_cif(struct simage *a, struct scifra *c)
{
    int i, j;
    for(i = 0; i < 5; i++)
    {
        for(j = 0; j < 5; j++)
        {
            if(cifre[c->cif_type][i + 1][j + 1] == '$')
            {
                a->px[c->ln + i][c->col + j] = c->color;
            }
        }
    }
}

void bonus(struct simage *a)
{
    int i, j, k, nrpct = 0;
    struct pixel_coord lista_puncte[10005];
    struct pixel_average avg_mat[IMSIZE][IMSIZE];
    unsigned char pctbool[IMSIZE][IMSIZE];
    memset(pctbool, 0, IMSIZE * IMSIZE);

    for(i = 0; i < a->bmpi.height; i++) //cautam punctele singure in poza
    {
        for(j = 0; j < a->bmpi.width; j++)
        {
            if(a->px[i][j].b != 255 || a->px[i][j].g != 255 || a->px[i][j].r != 255) //punct diferit de alb
            { //ne asiguram ca nu iesim din bordura si verificam daca vecinii sunt puncte albe
                if(i > 0)
                    if(a->px[i - 1][j].b != 255 || a->px[i - 1][j].g != 255 || a->px[i - 1][j].r != 255) continue;
                if(j > 0)
                    if(a->px[i][j - 1].b != 255 || a->px[i][j - 1].g != 255 || a->px[i][j - 1].r != 255) continue;
                if(i < a->bmpi.height - 1)
                    if(a->px[i + 1][j].b != 255 || a->px[i + 1][j].g != 255 || a->px[i + 1][j].r != 255) continue;
                if(j < a->bmpi.width - 1)
                    if(a->px[i][j + 1].b != 255 || a->px[i][j + 1].g != 255 || a->px[i][j + 1].r != 255) continue;

                pctbool[i][j] = 1;
                lista_puncte[nrpct].ln = i;
                lista_puncte[nrpct].col = j;
                lista_puncte[nrpct++].color = a->px[i][j];
            }
            else //punct alb
            {
                pctbool[i][j] = 1;
            }
        }
    }

    for(i = 0; i < a->bmpi.height; i++)
    {
        for(j = 0; j < a->bmpi.width; j++)
        {
            if(pctbool[i][j] == 0)
            {
                //initializare punct din matricea de medie
                avg_mat[i][j].b = a->px[i][j].b;
                avg_mat[i][j].g = a->px[i][j].g;
                avg_mat[i][j].r = a->px[i][j].r;
                avg_mat[i][j].num = 1;

                for(k = 0; k < nrpct; k++)
                {
                    if((lista_puncte[k].ln - i) * (lista_puncte[k].ln - i) + //daca punctul unei cifre se afla in raza de actiune a unui punct izolat
                       (lista_puncte[k].col - j) * (lista_puncte[k].col - j)
                       <= 49)
                    {
                        avg_mat[i][j].b += a->px[lista_puncte[k].ln][lista_puncte[k].col].b;
                        avg_mat[i][j].g += a->px[lista_puncte[k].ln][lista_puncte[k].col].g;
                        avg_mat[i][j].r += a->px[lista_puncte[k].ln][lista_puncte[k].col].r;
                        avg_mat[i][j].num += 1;
                    }
                }
            }
        }
    }

    for(i = 0; i < a->bmpi.height; i++)
    {
        for(j = 0; j < a->bmpi.width; j++)
        {
            if(pctbool[i][j] == 0)
            {
                a->px[i][j].b = avg_mat[i][j].b / avg_mat[i][j].num;
                a->px[i][j].g = avg_mat[i][j].g / avg_mat[i][j].num;
                a->px[i][j].r = avg_mat[i][j].r / avg_mat[i][j].num;
            }
        }
    }


}

int main()
{
    char saux[20], ssaux[50];
    char captchanamet1[30], captchanamet2[30], captchanamet3[30], captchanametb[30], *p_char;

    //citirea din input.txt
    in = fopen("input.txt", "r");

    fgets(saux, 20, in);
    saux[strlen(saux) - 1] = 0;
    strcpy(ssaux, saux);
    p_char = strtok(saux, ".");

    strcpy(captchanamet1, p_char);
    strcat(captchanamet1, "_task1.bmp");
    strcpy(captchanamet2, p_char);
    strcat(captchanamet2, "_task2.txt");
    strcpy(captchanamet3, p_char);
    strcat(captchanamet3, "_task3.bmp");

    //citirea imaginii

    iimg = fopen(ssaux, "rb");
    struct simage image_save;
    read_image(&image_save);
    fclose(iimg);

    //task 1
    struct simage image1;
    int i,j;
    struct pixel newpx;
    fscanf(in, "%d%d%d", &newpx.b, &newpx.g, &newpx.r);
    image1 = image_save;
    for(i = 0; i < image1.bmpi.height; i++) //parcurgem imaginea
    {
        for(j = 0; j < image1.bmpi.width; j++)
        {
            if(image1.px[i][j].b != 255 && image1.px[i][j].g != 255 && image1.px[i][j].r != 255)
            {
                image1.px[i][j] = newpx; //modificam culoarea pentru task1
                image_map[i + 1][j + 1] = '$'; //salvam o harta a imaginii pentru task2
            }
            else
                image_map[i + 1][j + 1] = '-';
        }
    }

    oimg = fopen(captchanamet1, "wb");
    write_image(&image1);
    fclose(oimg);

    //task 2

    struct scifra lista_cifre[401]; //lista si contor pentru task 3
    int nrcif = 0;

    for(i = 0; i <= image1.bmpi.height + 1; i++) //se bordeaza harta imaginii
    {
        image_map[i][0] = '-';
        image_map[i][image1.bmpi.width + 1] = '-';
    }
    for(j = 0; j <= image1.bmpi.width + 1; j++)
    {
        image_map[0][j] = '-';
        image_map[image1.bmpi.height + 1][j] = '-';
    }

    int cfcr;
    oimg = fopen(captchanamet2, "w");
    for(j = 0; j <= image1.bmpi.width - 5; j++) //parcurgem imaginea cu cadranul de verificare
    {
        for(i = 0; i <= image1.bmpi.height - 5; i++)
        {
            cfcr = check_area(i, j); //verificam daca zona corespunde unei cifre
            if(cfcr != -1)
            {
                fprintf(oimg, "%d", cfcr); //scriem direct pt task 2
                lista_cifre[nrcif].color = image_save.px[i + 4][j + 4]; //salvarea cifrei in lista pentru task 3
                lista_cifre[nrcif].cif_type = cfcr;
                lista_cifre[nrcif].ln = i;
                lista_cifre[nrcif].col = j;
                nrcif ++;
            }
        }
    }
    fclose(oimg);

    //task 3

    while(fscanf(in, "%d", &cfcr) == 1) //eliminarea cifrelor
    {
        for(i = nrcif - 1; i >= 0; i--)
        {
            if(lista_cifre[i].cif_type == cfcr)
            {
                for(j = i + 1; j < nrcif; j++)
                {
                    lista_cifre[j - 1].cif_type = lista_cifre[j].cif_type;
                    lista_cifre[j - 1].color = lista_cifre[j].color;
                }
                nrcif--;
            }
        }
    }

    struct simage image3; //crearea imaginii cu cifrele ramase in lista
    image3.bmpf = image_save.bmpf;
    image3.bmpi = image_save.bmpi;
    memset(image3.px, 255, sizeof(struct pixel) * IMSIZE * IMSIZE); //setam fundalul pe alb

    for(i = 0; i < nrcif; i++) //parcurgerea listei de cifre
    {
        draw_cif(&image3, &lista_cifre[i]); //desenam cifra
    }

    oimg = fopen(captchanamet3, "wb");
    write_image(&image3);
    fclose(oimg);



    //bonus

    fgets(saux, 20, in);
    saux[strlen(saux) - 1] = 0;
    strcpy(ssaux, saux);
    p_char = strtok(saux, ".");

    strcpy(captchanametb, p_char);
    strcat(captchanametb, "_bonus.bmp");

    iimg = fopen(ssaux, "rb");
    struct simage image_save_b, image_b;
    read_image(&image_save_b);
    fclose(iimg);

    image_b = image_save_b;
    bonus(&image_b);

    oimg = fopen(captchanametb, "wb");
    write_image(&image_b);
    fclose(oimg);

    return 0;
}
