// Voicu Alex-Georgian

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <complex.h>

int N, P;

double *dataT;
double complex *dataF;

void dft(void *tid)
{
	int low, hi;
	low = ceil((float) N / P) * *(int *)tid;
	hi = ceil((float) N / P) * (*(int *)tid + 1);
	if(hi > N)
		hi = N;

	double complex coef = (- 2 * M_PI / N) * I;

	for (int k = low; k < hi; k++)
	{
		dataF[k] = 0;
		for (int n = 0; n < N; n++)
		{
			dataF[k] += dataT[n] * cexp(coef * k * n);
		}
	}
}

int main(int argc, char * argv[]) {

	if (argc != 4)
	{
		printf("Use: <in file> <out file> <thread num>\n");
		return -1;
	}

	FILE *in, *out;

	in = fopen(argv[1], "r");
	if (in == NULL)
	{
		printf("Cannot open input file!\n");
		return -1;
	}

	out = fopen(argv[2], "w");
	if (out == NULL)
	{
		printf("Cannot open output file!\n");
		fclose(in);
		return -1;
	}

	P = atoi(argv[3]);

	fscanf(in, "%d", &N);

	dataT = malloc(sizeof(double) * N);
	if (dataT == NULL)
	{
		printf("Cannot malloc!\n");
		fclose(in);
		fclose(out);
		return -1;
	}

	dataF = malloc(sizeof(double complex) * N);
	if (dataF == NULL)
	{
		printf("Cannot malloc!\n");
		fclose(in);
		fclose(out);
		free(dataT);
		return -1;
	}

	for (int i = 0; i < N; i++)
	{
		fscanf(in, "%lf", dataT + i);
	}

	
	pthread_t threads[P];
	int tid[P];

	for (int i = 0; i < P; i++)
	{
		tid[i] = i;
		pthread_create(&(threads[i]), NULL, dft, (void *)(tid + i));
	}

	for (int i = 0; i < P; i++)
	{
		pthread_join(threads[i], NULL);
	}

	fprintf(out, "%d\n", N);
	for (int i = 0; i < N; i++)
	{
		fprintf(out, "%lf %lf\n", creal(dataF[i]), cimag(dataF[i]));
	}

	fclose(in);
	fclose(out);
	free(dataT);
	free(dataF);

	return 0;
}
