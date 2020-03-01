//Voicu Alex-Georgian

#include <stdio.h>
#include <stdlib.h>

using namespace std;

int n, k;
int a[1005][1005];
int v[1005];

int cmpDes(const void *a, const void *b)
{
	return *(int*)b - *(int*)a;
}

int main()
{
	FILE *f, *g;
	f = fopen("p2.in", "r");
	g = fopen("p2.out", "w");
	fscanf(f, "%d%d", &n, &k);
	for(int i = 0; i < n; i++)
	{
		fscanf(f, "%d", v + i);
	}
	if(k == n)
	{
		fprintf(g, "0");
		fclose(f);
		fclose(g);
		return 0;
	}
	qsort(v, n, sizeof(int), cmpDes);

	int slen = n - k;

	for(int i = 0; i <= k; i++)
	{
		a[i][0] = v[0];
	}
	for(int j = 1, sign = 1; j < slen; j++)
	{
		sign = -sign;
		a[0][j] = a[0][j - 1] + sign * v[j];
	}

	int i, j, sign, skip_s, take_s;
	for(i = 1; i <= k; i++)
	{
		for(j = 1; j < slen; j++)
		{
			if((j & 1) == 1)
				sign = -1;
			else
				sign = 1;
			skip_s = a[i - 1][j];
			take_s = a[i][j - 1] + sign * v[i + j];
			a[i][j] = skip_s > take_s ? skip_s : take_s;
		}
	}

	fprintf(g, "%d", a[k][slen - 1]);

	fclose(f);
	fclose(g);
	return 0;
}
