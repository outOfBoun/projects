// Voicu Alex-Georgian

#include <stdio.h>
#include <queue>

using namespace std;

#define NMAX 105

int n, m, k, vizcrt;
int mat[NMAX][NMAX], viz[NMAX][NMAX];
int dx[] = {-1, 0, 1, 0},
	dy[] = {0, -1, 0, 1};

int fillLeeBFS(pair<int, int> start)
{
	queue<pair<int, int>> q;
	int minVal, maxVal;

	minVal = mat[start.first][start.second];
	maxVal = minVal + k;

	q.push(start);
	vizcrt++;
	viz[start.first][start.second] = vizcrt;

	pair<int, int> nod, vec;
	int i, vecVal, count = 1;

	while(!q.empty())
	{
		nod = q.front();
		q.pop();
		for(i = 0; i < 4; i++)
		{
			vec.first = nod.first + dx[i];
			vec.second = nod.second + dy[i];
			vecVal = mat[vec.first][vec.second];

			if(vecVal == -1 || viz[vec.first][vec.second] == vizcrt)
				continue;
			if(vecVal < minVal || vecVal > maxVal)
				continue;

			count++;
			q.push(vec);
			viz[vec.first][vec.second] = vizcrt;
		}
	}
	return count;
}

int main()
{
	freopen("p2.in", "r", stdin);
	freopen("p2.out", "w", stdout);

	int i, j, maxsize = 1, maxsizecrt, maxpossibleans;
	scanf("%d%d%d", &n, &m, &k);
	maxpossibleans = n * m;

	//bordering cost matrix with -1
	for(i = 0; i <= n + 1; i++)
	{
		mat[i][0] = mat[i][m + 1] = -1;
	}
	for(i = 0; i <= m + 1; i++)
	{
		mat[0][i] = mat[n + 1][i] = -1;
	}

	for(i = 1; i <= n; i++)
	{
		for(j = 1; j <= m; j++)
		{
			scanf("%d", &(mat[i][j]));
		}
	}

	for(i = 1; i <= n; i++)
	{
		for(j = 1; j <= m; j++)
		{
			maxsizecrt = fillLeeBFS(make_pair(i,j));
			if(maxsizecrt > maxsize)
			{
				maxsize = maxsizecrt;
				if(maxsize == maxpossibleans)
					goto end;
			}
		}
	}

end:
	printf("%d\n", maxsize);

	return 0;
}
