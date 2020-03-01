//Voicu Alex-Georgian

#include <stdio.h>
#include <stdlib.h>

using namespace std;

int n;
int v[100005];

int cmpDes(const void *a, const void *b)
{
	return *(int*)b - *(int*)a;
}

int main()
{
	FILE *f, *g;
	f = fopen("p1.in", "r");
	g = fopen("p1.out", "w");
	fscanf(f, "%d", &n);
	for(int i = 0; i < n; i++)
	{
		fscanf(f, "%d", v + i);
	}
	qsort(v, n, sizeof(int), cmpDes);

	int avantaj = v[0];
	int sign = 1;
	for(int i = 1; i < n; i++)
	{
		sign = -sign;
		avantaj += sign * v[i];
	}

	fprintf(g, "%d", avantaj);

	fclose(f);
	fclose(g);
	return 0;
}
