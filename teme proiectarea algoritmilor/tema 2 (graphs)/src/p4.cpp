// Voicu Alex-Georgian

#include <stdio.h>
#include <tuple>
#include <vector>
#include <queue>
#include <set>
#include <unordered_map>

using namespace std;

#define NMAX 205
#define TMAX 8
#define INF 0x7fffffffffffffff

int n, m, t;

vector<tuple<int, int, int>> adj[NMAX]; //neighbour, cost, type

unordered_map<int, long long> distByTypes[NMAX];

int type_pen[TMAX];
char inq[NMAX];

void bford(int init)
{
	queue<int> q;
	q.push(init);
	inq[init] = 1;

	int nod, vec, cost, type, mtp, alltp;
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
			for(auto typecomb : distByTypes[nod]) // for all combinations of types of nodes that enter node
			{
				if(vec == 1) // don't improve node 1 (positive costs anyway)
					continue;
				mtp = 1<<type;
				alltp = mtp | typecomb.first;
				if((mtp & typecomb.first) > 0) // penalty for type already added so just add cost
				{
					if(distByTypes[vec].find(alltp) == distByTypes[vec].end() || 
						distByTypes[vec][alltp] > typecomb.second + cost)
					{
						distByTypes[vec][alltp] = typecomb.second + cost;
						if(!inq[vec])
						{
							inq[vec] = 1;
							q.push(vec);
						}
					}
				}
				else if(distByTypes[vec].find(alltp) == distByTypes[vec].end() || 
					distByTypes[vec][alltp] > typecomb.second + cost + type_pen[type])
				{
					distByTypes[vec][alltp] = typecomb.second + cost + type_pen[type];
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
	freopen("p4.in", "r", stdin);
	freopen("p4.out", "w", stdout);

	scanf("%d%d%d", &n, &m, &t);
	int i, u, v, x, y;

	for(i = 0; i < m; i++)
	{
		scanf("%d%d%d%d", &u, &v, &x, &y);
		adj[u].push_back(make_tuple(v, x, y));
		adj[v].push_back(make_tuple(u, x, y));
	}
	for(i = 1; i <= t; i++)
	{
		scanf("%d", &(type_pen[i]));
	}
	
	distByTypes[1][0] = 0; // only type that enters node 1 is of type 0 (not part of input types)

	bford(1);

	long long min = INF;
	for(auto typecomb : distByTypes[n])
	{
		if(min > typecomb.second)
			min = typecomb.second;
	}

	if(min == INF)
		min = -1;

	printf("%lld\n", min);

	return 0;
}
