// Voicu Alex-Georgian

#include <stdio.h>
#include <vector>
#include <queue>

using namespace std;

#define NMAX 100001

struct cmp
{
	bool operator() (const pair<int, int> &a, const pair<int, int> &b)
	{
		if(a.first == b.first)
			return a.second > b.second;
		return a.first > b.first;
	}
};

int n;
vector <int> adj[NMAX];

// priority queue containing pairs of (distanta, nod)
priority_queue <pair<int, int>, vector<pair<int, int>>, cmp> dist;

int main()
{
	freopen("p1.in", "r", stdin);
	freopen("p1.out", "w", stdout);

	int i, j, m = 0;
	scanf("%d", &n);

	for(i = 1; i <= n; i++)
	{
		scanf("%d", &j);
		dist.push(make_pair(j, i));
	}

	int crt_head = 1, crt_dist = 0, new_head = 0;
	dist.pop();
	
	while(!dist.empty())
	{
		pair<int, int> p;
		p = dist.top();

		if(crt_dist + 1 == p.first)
		{
			adj[crt_head].push_back(p.second);
			new_head = p.second;
			dist.pop();
			m++;
		}
		else if(crt_dist + 2 == p.first)
		{
			if(new_head == 0)
			{
				printf("-1\n");
				return 0;
			}
			crt_head = new_head;
			crt_dist++;
		}
		else
		{
			printf("-1\n");
			return 0;
		}
	}

	printf("%d\n", m);

	for(i = 1; i <= n; i++)
	{
		for(auto p : adj[i])
		{
			printf("%d %d\n", i, p);
		}
	}

	return 0;
}
