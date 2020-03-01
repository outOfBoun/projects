//Voicu Alex-Georgian

#include <stdio.h>
#define INF 9223372036854775807

using namespace std;

long long ar[1005][1005], at[1005][1005];
long long v[1005];
int n;

long long dp_tz(int st, int dr);
long long dp_rz(int st, int dr);

long long dp_tz(int st, int dr)
{
	if(at[st][dr] != INF)
		return at[st][dr];
	long long lchoice, rchoice;
	lchoice = v[st] + dp_rz(st + 1, dr);
	rchoice = dp_rz(st, dr - 1) + v[dr];
	at[st][dr] = lchoice > rchoice ? lchoice : rchoice;
	return at[st][dr];
}

long long dp_rz(int st, int dr)
{
	if(ar[st][dr] != INF)
		return ar[st][dr];
	long long lchoice, rchoice;
	lchoice = (- v[st]) + dp_tz(st + 1, dr);
	rchoice = dp_tz(st, dr - 1) + (- v[dr]);
	ar[st][dr] = lchoice < rchoice ? lchoice : rchoice;
	return ar[st][dr];
}

int main()
{
	FILE *f, *g;
	f = fopen("p3.in", "r");
	g = fopen("p3.out", "w");
	fscanf(f, "%d", &n);
	for(int i = 0; i < n; i++)
	{
		fscanf(f, "%d", v + i);
	}
	for(int i = 0; i < n; i++)
	{
		for(int j = i; j < n; j++)
		{
			if(i != j)
			{
				at[i][j] = INF;
				ar[i][j] = INF;
				continue;
			}	
			else
			{
				at[i][j] = v[i];
				ar[i][j] = -v[i];
			}
		}
	}

	dp_tz(0, n-1);
	
	fprintf(g, "%d\n", at[0][n-1]);

	fclose(f);
	fclose(g);
	return 0;
}
