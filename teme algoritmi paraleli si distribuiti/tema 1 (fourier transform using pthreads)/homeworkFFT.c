// Voicu Alex-Georgian

//cod FFT preluat de pe rosettacode
//modificat pt paralelizare

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <complex.h>

typedef double complex cplx;

struct fftparam{
	cplx *buf;
	cplx *out;
	int n;
	int step;
};

int N, P;

cplx *dataT;
cplx *dataF;


void _fft1(cplx buf[], cplx out[], int n, int step)
{
	if (step < n) 
	{
		_fft1(out, buf, n, step * 2);
		_fft1(out + step, buf + step, n, step * 2);

		cplx coef = -I * (double) M_PI / n;
 
		for (int i = 0; i < n; i += 2 * step) 
		{
			cplx t = cexp(coef * i) * out[i + step];
			buf[i / 2]     = out[i] + t;
			buf[(i + n)/2] = out[i] - t;
		}
	}
}
 
void fft1(cplx buf[], cplx out[], int n)
{
	for (int i = 0; i < n; i++) 
		out[i] = buf[i];

	_fft1(buf, out, n, 1);
}




void _fft2r(void * param)
{
	cplx *buf = ((struct fftparam *)param)->buf;
	cplx *out = ((struct fftparam *)param)->out;
	int n = ((struct fftparam *)param)->n;
	int step = ((struct fftparam *)param)->step;

	if (step < n) 
	{
		struct fftparam p1, p2;

		p1.buf = out; p1.out = buf; p1.n = n; p1.step = step * 2;
		_fft2r((void *)(&p1));

		p2.buf = out + step; p2.out = buf + step; p2.n = n; p2.step = step * 2;
		_fft2r((void *)(&p2));

		cplx coef = -I * (double) M_PI / n;

 
		for (int i = 0; i < n; i += 2 * step)
		{
			cplx t = cexp(coef * i) * out[i + step];
			buf[i / 2]     = out[i] + t;
			buf[(i + n)/2] = out[i] - t;
		}
	}
}
 
void fft2(cplx buf[], cplx out[], int n)
{
	for (int i = 0; i < n; i++) 
		out[i] = buf[i];

	pthread_t threads[2];

	int step = 1;
	if (step < n) 
	{
		struct fftparam p1, p2;

		p1.buf = out; p1.out = buf; p1.n = n; p1.step = step * 2;
		pthread_create(&(threads[0]), NULL, _fft2r, (void *)(&p1));

		p2.buf = out + step; p2.out = buf + step; p2.n = n; p2.step = step * 2;
		pthread_create(&(threads[1]), NULL, _fft2r, (void *)(&p2));

		cplx coef = -I * (double) M_PI / n;

		for (int i = 0; i < 2; i++)
		{
			pthread_join(threads[i], NULL);
		}
 
		for (int i = 0; i < n; i += 2 * step) 
		{
			cplx t = cexp(coef * i) * out[i + step];
			buf[i / 2]     = out[i] + t;
			buf[(i + n)/2] = out[i] - t;
		}
	}
}




void _fft4(void * param)
{
	cplx *buf = ((struct fftparam *)param)->buf;
	cplx *out = ((struct fftparam *)param)->out;
	int n = ((struct fftparam *)param)->n;
	int step = ((struct fftparam *)param)->step;

	if (step < n) 
	{
		struct fftparam p1, p2;

		pthread_t thread;

		p1.buf = out; p1.out = buf; p1.n = n; p1.step = step * 2;
		pthread_create(&(thread), NULL, _fft2r, (void *)(&p1));

		p2.buf = out + step; p2.out = buf + step; p2.n = n; p2.step = step * 2;
		_fft2r((void *)(&p2));

		cplx coef = -I * (double) M_PI / n;

		pthread_join(thread, NULL);
 
		for (int i = 0; i < n; i += 2 * step)
		{
			cplx t = cexp(coef * i) * out[i + step];
			buf[i / 2]     = out[i] + t;
			buf[(i + n)/2] = out[i] - t;
		}
	}
}
 
void fft4(cplx buf[], cplx out[], int n)
{
	for (int i = 0; i < n; i++) 
		out[i] = buf[i];

	pthread_t threads[2];

	int step = 1;
	if (step < n) 
	{
		struct fftparam p1, p2;

		p1.buf = out; p1.out = buf; p1.n = n; p1.step = step * 2;
		pthread_create(&(threads[0]), NULL, _fft4, (void *)(&p1));

		p2.buf = out + step; p2.out = buf + step; p2.n = n; p2.step = step * 2;
		pthread_create(&(threads[1]), NULL, _fft4, (void *)(&p2));

		cplx coef = -I * (double) M_PI / n;

		for (int i = 0; i < 2; i++)
		{
			pthread_join(threads[i], NULL);
		}
 
		for (int i = 0; i < n; i += 2 * step) 
		{
			cplx t = cexp(coef * i) * out[i + step];
			buf[i / 2]     = out[i] + t;
			buf[(i + n)/2] = out[i] - t;
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

	dataT = malloc(sizeof(cplx) * N);
	if (dataT == NULL)
	{
		printf("Cannot malloc!\n");
		fclose(in);
		fclose(out);
		return -1;
	}

	dataF = malloc(sizeof(cplx) * N);
	if (dataF == NULL)
	{
		printf("Cannot malloc!\n");
		fclose(in);
		fclose(out);
		free(dataT);
		return -1;
	}

	double aux;

	for (int i = 0; i < N; i++)
	{
		fscanf(in, "%lf", &aux);
		dataT[i] = aux;
	}


	if(P == 1)
	{
		fft1(dataT, dataF, N);
	}
	else if(P == 2)
	{
		fft2(dataT, dataF, N);
	}
	else if(P == 4)
	{
		fft4(dataT, dataF, N);
	}
	

	fprintf(out, "%d\n", N);
	for (int i = 0; i < N; i++)
	{
		fprintf(out, "%lf %lf\n", creal(dataT[i]), cimag(dataT[i]));
	}

	fclose(in);
	fclose(out);
	free(dataT);
	free(dataF);

	return 0;
}
