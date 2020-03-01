// Voicu Alex-Georgian

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

unsigned char *arena_p = NULL;
uint32_t arena_size, arena_size_cr, arena_start;

void initialize(int n)
{
    arena_p = calloc(n, 1);
    arena_size = n;
    arena_start = 0;
    arena_size_cr = 0;
}

void finalize()
{
    free(arena_p);
    arena_p = NULL;
}

void dump()
{
    int i, j, last_column, min, full_space;
    full_space = (arena_size >> 4) << 4; //numarul total de octeti care umplu un spatiu dreptunghiular
    last_column = arena_size - full_space; //numarul de octeti de pe ultima linie in cazul in care arena size nu e un multiplu de 16

    for(i = 0; i < full_space; i+=16)
    {
        printf("%08X\t", i);
        for(j = 0; j < 8 ; j++)
        {
            printf("%02X ", arena_p[i + j]);
        }
        printf(" ");
        for(j = 8; j < 16 ; j++)
        {
            printf("%02X ", arena_p[i + j]);
        }
        printf("\n");
    }

    if(!last_column) return;
    min = last_column < 8 ? last_column : 8;
    printf("%08X\t", i);
    for(j = 0; j < min ; j++)
    {
        printf("%02X ", arena_p[i + j]);
    }
    if(min < 8) {printf("\n");return;}
    printf(" ");
    for(j = 8; j < last_column ; j++)
    {
        printf("%02X ", arena_p[i + j]);
    }
    printf("\n");
}

void freemeu(uint32_t index)
{
    index -= 12;
    arena_size_cr += -(*(uint32_t*)(arena_p + index + 8)) - 12; //in toate cazurile reducem variabila ce tine cont de memoria alocata la un moment dat
    if(arena_size_cr == 0) //singurul bloc din arena
    {
        arena_start = 0;
    }
    else if(index == arena_start) //primul bloc
    {
        arena_start = (*(uint32_t*)(arena_p + index + 0)); //indexul de start va fi indexul blocului secund
        (*(uint32_t*)(arena_p + (*(uint32_t*)(arena_p + index + 0)) + 4)) = 0; //blocul secund nu mai are predecesor
    }
    else if((*(uint32_t*)(arena_p + index + 0)) == 0) //ultimul bloc
    {
        (*(uint32_t*)(arena_p + (*(uint32_t*)(arena_p + index + 4)) + 0)) = 0; //blocul penultim nu mai are succesor
    }
    else //bloc inserat intre alte doua
    {
        //indexul de predecesor al blocului urmator va fi indexul de predecesor al blocului curent
        (*(uint32_t*)(arena_p + (*(uint32_t*)(arena_p + index + 0)) + 4)) = (*(uint32_t*)(arena_p + index + 4));
        //indexul de succesor al blocului precedent va fi indexul de succesor al blocului curent
        (*(uint32_t*)(arena_p + (*(uint32_t*)(arena_p + index + 4)) + 0)) = (*(uint32_t*)(arena_p + index + 0));
    }

    memset(arena_p + index, 0, (*(uint32_t*)(arena_p + index + 8)) + 12); //in toate cazurile stergem memoria alocata blocului
}

void add_block(uint32_t current_index, uint32_t pred_index, uint32_t size)
{
    //indexul de succesor al blocului curent va fi indexul de succesor al blocului precedent
    //indexul de predecesor al blocului curent va fi indexul blocului precedent
    *(uint32_t*)(arena_p + current_index + 0) = *(uint32_t*)(arena_p + pred_index);
    *(uint32_t*)(arena_p + current_index + 4) = pred_index;
    *(uint32_t*)(arena_p + current_index + 8) = size;

    //indexul de predecesor al blocului urmator blocului precedent va fi indexul blocului curent
    *(uint32_t*)(arena_p + (*(uint32_t*)(arena_p + pred_index + 0)) + 4) = current_index;

    //indexul de succesor al blocului precedent va fi indexul blocului curent
    *(uint32_t*)(arena_p + pred_index) = current_index;

    arena_size_cr += size + 12;
}

void add_block_at_end(uint32_t current_index, uint32_t pred_index, uint32_t size)
{

    //indexul de succesor al blocului curent va fi indexul de succesor al blocului precedent
    //indexul de predecesor al blocului curent va fi indexul blocului precedent
    *(uint32_t*)(arena_p + current_index + 0) = *(uint32_t*)(arena_p + pred_index);
    *(uint32_t*)(arena_p + current_index + 4) = pred_index;
    *(uint32_t*)(arena_p + current_index + 8) = size;

    //indexul de succesor al blocului precedent va fi indexul blocului curent
    *(uint32_t*)(arena_p + pred_index) = current_index;

    arena_size_cr += size + 12;
}

uint32_t alloc(uint32_t n)
{
    if(n + 12 > arena_size) {printf("0\n"); return 0;} //daca nu e suficient spatiu pentru a aloca in arena

    uint32_t p_next = arena_start, p_cr;
    p_cr = p_next;

    //verificam daca exista spatiu suficient inainte de primul element sau daca nu exista niciun bloc
    if((int)(p_cr - n - 12) >= 0 ||
       (arena_size_cr == 0 && (int)(arena_size - n - 12) >= 0))
    {
        //atunci inseram la inceputul lantului pe index = 0
        *(uint32_t*)(arena_p + 0 + 0) = arena_start;
        *(uint32_t*)(arena_p + 0 + 4) = 0;
        *(uint32_t*)(arena_p + 0 + 8) = n;
        *(uint32_t*)(arena_p + arena_start + 4) = 0;

        arena_start = 0;
        arena_size_cr += n + 12;
        printf("12\n");

        return 12;
    }

    //altfel parcurgem lista blocurilor pana cand gasim un spatiu suficient de mare sau ajungem la ultimul element
    p_next =  *(uint32_t*)(arena_p + p_cr + 0);
    while(p_next != 0)
    {
        if((int)(p_next - p_cr - n - (*(uint32_t*)(arena_p + p_cr + 8)) - 24) >= 0)
        {
            add_block(p_cr + (*(uint32_t*)(arena_p + p_cr + 8)) + 12, p_cr, n);
            printf("%d\n", p_cr + (*(uint32_t*)(arena_p + p_cr + 8)) + 12 + 12);
            return p_cr + (*(uint32_t*)(arena_p + p_cr + 8)) + 12 + 12;
        }
        p_cr = p_next;
        p_next =  (*(uint32_t*)(arena_p + p_cr + 0));
    }

    //daca nu a fost gasit loc liber intre blocurile existente, atunci adaugam la final daca este loc
    if((int)(arena_size - p_cr - n - (*(uint32_t*)(arena_p + p_cr + 8)) - 24) >= 0)
    {
        add_block_at_end(p_cr + (*(uint32_t*)(arena_p + p_cr + 8)) + 12, p_cr, n);
        printf("%d\n", p_cr + (*(uint32_t*)(arena_p + p_cr + 8)) + 12 + 12);
        return p_cr + (*(uint32_t*)(arena_p + p_cr + 8)) + 12 + 12;
    }

    printf("0\n");
    return 0;
}

uint32_t find_alligned(uint32_t n, uint32_t align) //folosita la alloc_alligned; gaseste primul index care este aliniat la align
{
    if(align == 0) return n;
    if(((n >> align) << align ) == n) return n;
    return ((n >> align) + 1) << align;
}

uint32_t alloc_alligned(uint32_t n, uint32_t alignnum)
{
    uint32_t align = 0; //biti de 0 pt aliniere
    while(alignnum > 1)
    {
        alignnum = alignnum >> 1;
        align++;
    }
    uint32_t aliniat = find_alligned(12, align) - 12; //indexul de start aliniat minus sectiunea de gestiune

    if(n + 12 + aliniat > arena_size) {printf("0\n");return 0;}

    uint32_t p_next = arena_start, p_cr;
    p_cr = p_next;

    //verificam daca exista spatiu suficient inainte de primul element sau daca nu exista niciun bloc
    if((int)(p_cr - n - 12 - aliniat) >= 0 ||
       (arena_size_cr == 0 && (int)(arena_size - n - 12 - aliniat) >= 0))
    {
        //atunci inseram la inceputul lantului pe index = aliniat
        *(uint32_t*)(arena_p + aliniat + 0) = arena_start;
        *(uint32_t*)(arena_p + aliniat + 4) = 0;
        *(uint32_t*)(arena_p + aliniat + 8) = n;
        if(arena_size_cr != 0) *(uint32_t*)(arena_p + arena_start + 4) = aliniat; //daca e singurul element atunci nu realizam vreo legatura

        arena_start = aliniat;
        arena_size_cr += n + 12;
        printf("%d\n", aliniat + 12);

        return aliniat + 12;
    }

    //altfel parcurgem lista blocurilor pana cand gasim un spatiu suficient de mare sau ajungem la ultmul bloc
    p_next =  *(uint32_t*)(arena_p + p_cr + 0);
    while(p_next != 0)
    {
        aliniat = find_alligned(p_cr + 24 + (*(uint32_t*)(arena_p + p_cr + 8)), align) - 12; //cautam primul index aliniat

        if((int)(p_next - aliniat - n - 12) >= 0)
        {
            add_block(aliniat, p_cr, n);
            printf("%d\n", aliniat + 12);
            return aliniat + 12;
        }
        p_cr = p_next;
        p_next =  (*(uint32_t*)(arena_p + p_cr + 0));
    }

    //daca nu a fost gasit loc liber intre blocurile existente, atunci adaugam la final daca este loc
    aliniat = find_alligned(p_cr + 24 + (*(uint32_t*)(arena_p + p_cr + 8)), align) - 12;
    if((int)(arena_size - aliniat - n - 12) >= 0)
    {
        add_block_at_end(aliniat, p_cr, n);
        printf("%d\n", aliniat + 12);
        return aliniat + 12;
    }

    printf("0\n");
    return 0;
}

void reallocmeu(uint32_t index, uint32_t size)
{
    index -= 12;
    uint32_t p_nou=0;
    uint32_t old_size = *(uint32_t*)(arena_p + index + 8);

    uint32_t i,k;
    k = size < old_size ? size : old_size;
    k += 12;
    unsigned char aux[k];

    for(i = 12; i < k; i++)
        aux[i] = arena_p[index + i];

    freemeu(index + 12); //scoatem blocul din lista
    p_nou = alloc(size); //alocam un bloc nou
    if(p_nou == 0) {return;} //daca nu se aloca atunci terminam realloc

    p_nou -= 12;
    for(i = 12; i < k; i++)
        arena_p[p_nou + i] = aux[i];

    return;
}

void fillar(uint32_t index, int size, unsigned char value)
{
    int size_cr;
    index -= 12;
    do 
    {
        size_cr = (*(uint32_t*)(arena_p + index + 8));
        memset(arena_p + index + 12, value, size_cr <= size ? size_cr : size);
        size -= size_cr; //size devine variabila contor
        index = (*(uint32_t*)(arena_p + index + 0));
    }
    while(size > 0 && index != 0); //setam octetii din memorie car timp mai avem blocuri si nu i-am setat pe toti
}

void parse_command(char* cmd)
{
    const char* delims = " \n";

    char* cmd_name = strtok(cmd, delims);
    if (!cmd_name) {
        goto invalid_command;
    }

    if (strcmp(cmd_name, "INITIALIZE") == 0) {
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        uint32_t size = atoi(size_str);
        initialize(size);

    } else if (strcmp(cmd_name, "FINALIZE") == 0) {
        finalize();

    } else if (strcmp(cmd_name, "DUMP") == 0) {
        dump();

    } else if (strcmp(cmd_name, "ALLOC") == 0) {
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        uint32_t size = atoi(size_str);
        alloc(size);

    } else if (strcmp(cmd_name, "FREE") == 0) {
        char* index_str = strtok(NULL, delims);
        if (!index_str) {
            goto invalid_command;
        }
        uint32_t index = atoi(index_str);
        freemeu(index);

    } else if (strcmp(cmd_name, "FILL") == 0) {
        char* index_str = strtok(NULL, delims);
        if (!index_str) {
            goto invalid_command;
        }
        uint32_t index = atoi(index_str);
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        uint32_t size = atoi(size_str);
        char* value_str = strtok(NULL, delims);
        if (!value_str) {
            goto invalid_command;
        }
        uint32_t value = atoi(value_str);
        fillar(index, size, value);

    } else if (strcmp(cmd_name, "ALLOCALIGNED") == 0) {
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        uint32_t size = atoi(size_str);
        char* align_str = strtok(NULL, delims);
        if (!align_str) {
            goto invalid_command;
        }
        uint32_t align = atoi(align_str);
        alloc_alligned(size, align);

    } else if (strcmp(cmd_name, "REALLOC") == 0) {
        //printf("Found cmd REALLOC\n");
        char* index_str = strtok(NULL, delims);
        if (!index_str) {
            goto invalid_command;
        }
        uint32_t index = atoi(index_str);
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        uint32_t size = atoi(size_str);
        reallocmeu(index, size);

    } else {
        goto invalid_command;
    }

    return;

invalid_command:
    printf("Invalid command: %s\n", cmd);
    finalize();
    exit(1);
}

int main(void)
{
    ssize_t read;
    char* line = NULL;
    size_t len;

    /* parse input line by line */
    while ((read = getline(&line, &len, stdin)) != -1) {
        /* print every command to stdout */
        printf("%s", line);

        parse_command(line);
    }

    free(line);

    return 0;
}
