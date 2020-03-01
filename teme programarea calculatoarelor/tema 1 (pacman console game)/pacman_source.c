// Voicu Alex-Georgian

#include <stdio.h>

int modul(int a)
{
	if(a<0) return -a;
	return a;
}

void bordare_linie(int M)
{
	int i;
	for(i = 0; i <= M+1; i++)
			printf("*");
		printf("\n");
}

char orientare_decod(int x) //decodificare orientare
{
	if(x == 0) return 'v';
	else if(x == 1) return '>';
	else if(x == 2) return '^';
	else if(x == 3) return '<';
	return 0;
}

int verif_border(int N, int M, int x, int y, int **win) //verifica daca mutarea nu paraseste tabla
{
	if(x==M && y==N) //conditia de castig
	{
		(**win) = 1; 
		return 1;
	}
	
	//verificari pentru iesirea din bordura
	if(x < 1 || x > M) return 0;
	if(y < 1 || y > N) return 0;
	
	return 1; //1 daca totul e in regula (nu se iese din tabla)
}

void muta_jucator(char mutare, int N, int M, int *x, int *y, int *win, int *orientare) //produce mutarea, orienteaza jucatorul si afla daca a castigat
{	
	if(mutare == 's' && verif_border(N, M, *x, (*y)+1, &win))
	{	
		(*y)++; 
		(*orientare) =  0;	
	}
			
	if(mutare == 'd' && verif_border(N, M, (*x)+1, *y, &win))
	{	
		(*x)++; 
		(*orientare) = 1;
	}
			
	if(mutare == 'w' && verif_border(N, M, *x, (*y)-1, &win))
	{		
		(*y)--; 
		(*orientare) = 2;
	}	
		
	if(mutare == 'a' && verif_border(N, M, (*x)-1, *y, &win))
	{	
		(*x)--; 
		(*orientare) = 3;
	}
	
	return; //caz default, orientarea ramane la fel		
}

void schimba_min(int *min, int *lmin, int *cmin, int x, int y, int l, int c) //functie folosita la muta_obstacol pentru a minimiza distanta
{
	int auxmin;
	auxmin = modul(l - y) + modul(c - x);
	if(auxmin < *min)
	{
		*min = auxmin;
		*lmin = l;
		*cmin = c;
	}
}

void muta_obstacol(int *l, int *c, int x, int y)
{
	int min, lmin = (*l) - 1, cmin = (*c) - 1; 
	min = modul(lmin - y) + modul(cmin - x); //se initializeaza minimul cu mutarea stanga sus.
	
	//se cauta sa se minimizeze prin restul de 7 pozitii posibile
	//daca se gaseste o mutare mai buna, se salveaza noul minim
	schimba_min(&min, &lmin, &cmin, x, y, (*l) - 1, (*c));     // mijloc sus
	schimba_min(&min, &lmin, &cmin, x, y, (*l) - 1, (*c) + 1); // dreapta sus
	
	schimba_min(&min, &lmin, &cmin, x, y, (*l), (*c) - 1);     // stanga mijloc
	schimba_min(&min, &lmin, &cmin, x, y, (*l), (*c));         // mijloc mijloc
	schimba_min(&min, &lmin, &cmin, x, y, (*l), (*c) + 1);     // dreapta mijloc
	
	schimba_min(&min, &lmin, &cmin, x, y, (*l) + 1, (*c) - 1); // stanga jos
	schimba_min(&min, &lmin, &cmin, x, y, (*l) + 1, (*c));     // mijloc jos
	schimba_min(&min, &lmin, &cmin, x, y, (*l) + 1, (*c) + 1); // dreapta jos
	
	*l = lmin; //se muta obstacolul pe coordonatele gasite ca fiind bune (distanta e minima)
	*c = cmin;
}

int main()
{
	int N, M, x, y, P;
	int i, j, running = 0, orientare_cr = 0, win = 0, alterneaza = 0;
	char mutare = 0, chtoput;
	
	char m1,m2,m3;
	int l1,l2,l3,c1,c2,c3;
	
	scanf("%d%d%d%d%d", &N, &M, &x, &y, &P);
	
	if(P>=1) scanf("%d %d %c", &l1, &c1, &m1);
	if(P>=2) scanf("%d %d %c", &l2, &c2, &m2);
	if(P==3) scanf("%d %d %c", &l3, &c3, &m3);
	
	//afisare initiala
	bordare_linie(M);
	for(i = 1; i <= N; i++) //afisarea tablei
	{
		printf("*");	
		for(j = 1; j <= M; j++)
		{
			chtoput=' ';
				
			if(i == y && j == x) //s-a ajuns la pozitia jucatorului?
				chtoput = orientare_decod(orientare_cr); //daca da, se marcheaza pe tabla
			if(P>0) //se marcheaza obstacolele (daca exista)
			{
				if(P>=1)
					if(l1 == i && c1 == j)
						chtoput = 'x';
				if(P>=2)
					if(l2 == i && c2 == j)
						chtoput = 'x';
				if(P>=3)
					if(l3 == i && c3 == j)
						chtoput = 'x';
			}
			printf("%c",chtoput);
		}
		printf("*\n");
	}
	bordare_linie(M);
	if(x==M && y==N) running = 1;
	
	// aici incepe efectiv bucla jocului
	while(running == 0 && scanf("\n%c", &mutare)==1) //ruleaza atata timp cat nu s-a pierdut(cod running 2/3) sau nu s-a castigat(cod running 1) si se citesc litere
	{	
		if(mutare == 'q') break;
		
		muta_jucator(mutare, N, M, &x, &y, &win, &orientare_cr); //muta jucatorul, verifica conditii
		
		//se verifica daca jucatorul da peste un obstacol (cod running 2)
		if(P >= 1)
			if(y == l1 && x == c1) running = 2;
		if(P >= 2)
			if(y == l2 && x == c2) running = 2;
		if(P == 3)
			if(y == l3 && x == c3) running = 2;
		
		if(alterneaza == 1)	//se muta obstacolele mobile daca le-a venit randul
		{
			if(P >= 1)
			{
				if(m1 == 'm')
					muta_obstacol(&l1, &c1, x, y);
			}		
			if(P >= 2)
			{
				if(m2 == 'm')
					muta_obstacol(&l2, &c2, x, y);
			}		
			if(P >= 3)
			{
				if(m3 == 'm')
					muta_obstacol(&l3, &c3, x, y);
			}	
		}
			
		//se verifica daca un obstacol mobil da peste jucator (cod running 2)
		if(P >= 1 && m1 == 'm')
			if(y == l1 && x == c1) running = 2;
		if(P >= 2 && m2 == 'm')
			if(y == l2 && x == c2) running = 2;
		if(P == 3 && m3 == 'm')
			if(y == l3 && x == c3) running = 2;
			
		if(win == 1) //jucatorul a castigat
		{	
			if(running != 2) //daca jucatorul nu a dat peste un obstacol atunci castiga
				running = 1;
		}
		
		if(running == 2) break; //daca jucatorul a pierdut nu mai afisam ultima data

		bordare_linie(M);
		for(i = 1; i <= N; i++) //afisarea tablei
		{
			printf("*");
			
			for(j = 1; j <= M; j++)
			{
				chtoput=' '; //by default se pune spatiu
				
				if(i == y && j == x) //s-a ajuns la pozitia jucatorului?
					chtoput = orientare_decod(orientare_cr); //daca da, se marcheaza pe tabla
				if(P>0) //se marcheaza obstacolele (daca exista)
				{
					if(P>=1)
						if(l1 == i && c1 == j)
							chtoput = 'x';
					if(P>=2)
						if(l2 == i && c2 == j)
							chtoput = 'x';
					if(P>=3)
						if(l3 == i && c3 == j)
							chtoput = 'x';
				}
				printf("%c",chtoput);
			}
			printf("*\n");
		}
		bordare_linie(M);
		
		alterneaza=!alterneaza; //alterneaza randul obstacolelor mobile
	}
	
	if(running == 1) printf("GAME COMPLETED\n");
	if(running == 2) printf("GAME OVER\n");
	
	return 0;
}
