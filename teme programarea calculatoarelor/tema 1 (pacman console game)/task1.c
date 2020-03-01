// Voicu Alex-Georgian

#include <stdio.h>

int main()
{
	int N, i, j;
	char C;
	
	scanf("%d %c",&N, &C);
	
	for(i = 0; i < N; i++)
	{
		for(j = 0; j < i; j++)
		{
			printf("  ");
		}
		printf("%c",C); // separat pentru a obtine formatul asa cum se doreste in problema
		for(j = i+1; j < N; j++)
		{
			printf(" %c",C);
		}
		printf("\n");
	}
	
	return 0;
}
