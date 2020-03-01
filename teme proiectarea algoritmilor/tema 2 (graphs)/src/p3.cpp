// Voicu Alex-Georgian

#include <stdio.h>
#include <tuple>
#include <vector>
#include <queue>
#include <set>

using namespace std;

#define NMAX 205
#define TMAX 105
#define INF 0x7fffffffffffffff

int n, m, t;

vector<tuple<int, int, int>> adj[NMAX]; //neighbour, cost, type
set<int> node_edget[NMAX];

int type_pen[TMAX][TMAX];
long long dist_types[NMAX][TMAX];
char inq[NMAX];

void bford(int init)
{
	queue<int> q;
	q.push(init);
	inq[init] = 1;

	int nod, vec, cost, type;
	while(!q.empty())
	{
		nod = q.front();
		q.pop();
		inq[nod] = 0;
		for(auto &a : adj[nod])
		{
			vec = get<0>(a);
			cost = get<1>(a);
			type = get<2>(a);

			for(auto edget : node_edget[nod])
			{
				if(nod != 1) // don't apply penalty to 1's neighbours
				{
					if(dist_types[vec][type] > dist_types[nod][edget] + cost + type_pen[edget][type])
					{
						dist_types[vec][type] = dist_types[nod][edget] + cost + type_pen[edget][type];
						node_edget[vec].insert(type);
						if(!inq[vec])
						{
							inq[vec] = 1;
							q.push(vec);
						}
					}
				}
				else if(dist_types[vec][type] > cost)
				{
					dist_types[vec][type] = cost;
					node_edget[vec].insert(type);
					if(!inq[vec])
					{
						inq[vec] = 1;
						q.push(vec);
					}
				}
			}
			
		}
	}
}

int main()
{
	freopen("p3.in", "r", stdin);
	freopen("p3.out", "w", stdout);

	scanf("%d%d%d", &n, &m, &t);
	int i, j, u, v, x, y;

	for(i = 0; i < m; i++)
	{
		scanf("%d%d%d%d", &u, &v, &x, &y);
		adj[u].push_back(make_tuple(v, x, y));
		adj[v].push_back(make_tuple(u, x, y));
	}
	for(i = 1; i <= t; i++)
	{
		for(j = 1; j <= t; j++)
		{
			scanf("%d", &(type_pen[i][j]));
		}
	}
	for(i = 1; i <= n; i++)
	{
		for(j = 1; j <= t; j++)
		{
			dist_types[i][j] = INF;
		}
	}
	
	for(j = 1; j <= t; j++)
	{
		dist_types[1][j] = 0;
		node_edget[1].insert(j);
	}

	bford(1);

	long long min = INF;
	for(j = 1; j <= t; j++)
	{
		if(min > dist_types[n][j])
			min = dist_types[n][j];
	}

	if(min == INF)
		min = -1;

	printf("%lld\n", min);

	return 0;
}
