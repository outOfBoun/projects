// VOICU Alex-Georgian

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include <string.h>
#include <unordered_map>

using namespace std;

FILE *fin, *fout;

char Ptype[3], msg[256];
int width, height;
int maxval;

vector<string> filterList;

char **photoBW, **photoR, **photoG, **photoB;
char **photoBWrez, **photoRrez, **photoGrez, **photoBrez;

struct filter_s{
	float mat[3][3];
}smooth_f, blur_f, sharpen_f, mean_f, emboss_f;

unordered_map<string, filter_s> filterMap;

//for mpi
int rankpr;
int nProcesses, allnProcesses;
int Prow_start[100], Prow_end[100];


char** allocByteMatrix(int w, int h)
{
	char **v;
	v = new char*[h];
	if(v == NULL)
	{
		printf("Couldn't alloc!");
		return NULL;
	}

	for(int i = 0; i < h; i++)
	{
		v[i] = new char[w]();
		if(v[i] == NULL)
		{
			printf("Couldn't alloc!");
			for(int j = 0; j < i; j++)
			{
				delete[] v[j];
			}
			delete[] v; 
			return NULL;
		}
	}

	return v;
}

void deleteByteMatrix(char **v, int w, int h)
{
	if(v == NULL)
		return;
	for(int i = 0; i < h; i++)
	{
		delete[] v[i];
	}

	delete[] v;
}

void applyFilter(char **inchar, char **out, int w, int h, filter_s flt)
{
	unsigned char ** in = (unsigned char**) inchar;
	for(int i = 1; i <= height; i++)
	{
		for(int j = 1; j <= width; j++)
		{
			float aux = 0;
			aux = (float)(in[i - 1][j - 1])			* flt.mat[2][2];
			aux += (float)(in[i - 1][j])			* flt.mat[2][1];
			aux += (float)(in[i - 1][j + 1])		* flt.mat[2][0];
			aux += (float)(in[i][j - 1])			* flt.mat[1][2];
			aux += (float)(in[i][j]) 				* flt.mat[1][1];
			aux += (float)(in[i][j + 1])			* flt.mat[1][0];
			aux += (float)(in[i + 1][j - 1])		* flt.mat[0][2];
			aux += (float)(in[i + 1][j])			* flt.mat[0][1];
			aux += (float)(in[i + 1][j + 1])		* flt.mat[0][0];
					
			if(aux < 0)
			{
				aux = 0;
			}
			else if(aux > maxval)
			{
				aux = maxval;
			}

			out[i][j] = (unsigned char)aux;
		}
	}
	
}

void transfCode(int &c)
{
	MPI_Bcast(&c, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

int computeParallelPhoto(char **in, char **&out)
{
	/*
	Initial:
		0 - main will grant a chunk of the image to each worker (initial) or after (3)
	Filter await:
		1 - main will broadcast the filter to be used after (0) or (2)
	After compute:
		2 - main requests border
		3 - main requests whole chunk
	After gathering:
		4 - shutdown signal after (3)
	*/
	int send_code;
	MPI_Status status;

	char fltName[50];

	
	send_code = 0;
	transfCode(send_code);

	for(int i = 1; i <= nProcesses; i++)
	{
		for(int j = Prow_start[i] - 1; j <= Prow_end[i] + 1; j++)
		{
			MPI_Send(in[j], width + 2, MPI_CHAR, i, 0, MPI_COMM_WORLD);
		}
	}


	vector<string>::const_iterator itr = filterList.begin();
	while(itr != filterList.end())
	{
		send_code = 1;
		transfCode(send_code);

		strncpy(fltName, (*itr).c_str(), 49);
		fltName[49] = 0;
		MPI_Bcast(fltName, 50, MPI_CHAR, 0, MPI_COMM_WORLD);

		send_code = 2;
		transfCode(send_code);

		for(int i = 1; i <= nProcesses; i++)
		{
			MPI_Recv(out[Prow_start[i]], width + 2, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(out[Prow_end[i]], width + 2, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
		}
		for(int i = 1; i <= nProcesses; i++)
		{
			MPI_Send(out[Prow_start[i] - 1], width + 2, MPI_CHAR, i, 0, MPI_COMM_WORLD);
			MPI_Send(out[Prow_end[i] + 1], width + 2, MPI_CHAR, i, 0, MPI_COMM_WORLD);
		}

		itr++;
	}

	send_code = 3;
	transfCode(send_code);

	for(int i = 1; i <= nProcesses; i++)
	{
		for(int j = Prow_start[i]; j <= Prow_end[i]; j++)
		{
			MPI_Recv(out[j], width + 2, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
		}
	}


	return 0;
}

void workerComputeParallelPhoto()
{
	char **in, **out;
	in = allocByteMatrix(width + 2, height + 2);
	out = allocByteMatrix(width + 2, height + 2);

	char fltName[50];

	MPI_Status status;
	int recv_code;
	transfCode(recv_code);

	if(recv_code == 0)
	do
	{

	for(int j = 0; j <= height + 1; j++)
	{
		MPI_Recv(in[j], width + 2, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
	}

	bool swapb = false;
	transfCode(recv_code);
	if(recv_code == 3)
	{
		char **aux;
		aux = in; in = out; out = aux;
	}
	else
	while(recv_code == 1)
	{
		MPI_Bcast(fltName, 50, MPI_CHAR, 0, MPI_COMM_WORLD);

		if(swapb)
		{
			char **aux;
			aux = in; in = out; out = aux;
		}

		applyFilter(in, out, width, height, filterMap[string(fltName)]);

		//send borders, get borders
		transfCode(recv_code);
		if(recv_code == 2)
		{
			MPI_Ssend(out[1], width + 2, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
			MPI_Ssend(out[height], width + 2, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
			MPI_Recv(out[0], width + 2, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(out[height + 1], width + 2, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
		}

		transfCode(recv_code);
		swapb = true;
	}

	if(recv_code == 3)
	{
		for(int j = 1; j <= height; j++)
		{
			MPI_Ssend(out[j], width + 2, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
		}
	}

	transfCode(recv_code);
	}
	while(recv_code != 4);
	

	deleteByteMatrix(in, width + 2, height + 2);
	deleteByteMatrix(out, width + 2, height + 2);
}

int p5routine(char **in, char **&out)
{	
	in = allocByteMatrix(width + 2, height + 2);
	if(in == NULL)
	{
		return -1;
	}

	for(int i = 1; i <= height; i++)
	{
		for(int j = 1; j <= width; j++)
		{
			fscanf(fin, "%c", &(in[i][j]));
		}
	}

	out = allocByteMatrix(width + 2, height + 2);

	if(nProcesses >= 1)
	{
		int err = computeParallelPhoto(in, out);

		int send_code = 4;
		transfCode(send_code);

		deleteByteMatrix(in, width + 2, height + 2);
		return err;
	}

	vector<string>::const_iterator itr = filterList.begin();
	if(itr != filterList.end())
	{
		applyFilter(in, out, width, height, filterMap[*itr]);
		itr++;
		while(itr != filterList.end())
		{
			char **aux;
			aux = in;
			in = out;
			out = aux;
			applyFilter(in, out, width, height, filterMap[*itr]);
			itr++;
		}
	}
	else
	{
		for(int i = 1; i <= height; i++)
		{
			for(int j = 1; j <= width; j++)
			{
				out[i][j] = in[i][j];
			}
		}
	}
	deleteByteMatrix(in, width + 2, height + 2);

	return 0;
}

int p6routine(char **inR, char **&outR, char **inG, char **&outG, char **inB, char **&outB)
{
	inR = allocByteMatrix(width + 2, height + 2);
	if(inR == NULL)
	{
		return -1;
	}
	inG = allocByteMatrix(width + 2, height + 2);
	if(inG == NULL)
	{
		deleteByteMatrix(inR, width + 2, height + 2);
		return -1;
	}
	inB = allocByteMatrix(width + 2, height + 2);
	if(inB == NULL)
	{
		deleteByteMatrix(inR, width + 2, height + 2);
		deleteByteMatrix(inG, width + 2, height + 2);
		return -1;
	}
	
	for(int i = 1; i <= height; i++)
	{
		for(int j = 1; j <= width; j++)
		{
			fscanf(fin, "%c%c%c", &(inR[i][j]), &(inG[i][j]), &(inB[i][j]));
		}
	}

	outR = allocByteMatrix(width + 2, height + 2);
	outG = allocByteMatrix(width + 2, height + 2);
	outB = allocByteMatrix(width + 2, height + 2);

	if(nProcesses >= 1)
	{
		int err = computeParallelPhoto(inR, outR) |
				computeParallelPhoto(inG, outG) |
				computeParallelPhoto(inB, outB);

		int send_code = 4;
		transfCode(send_code);

		deleteByteMatrix(inR, width + 2, height + 2);
		deleteByteMatrix(inG, width + 2, height + 2);
		deleteByteMatrix(inB, width + 2, height + 2);
		return err;
	}

	vector<string>::const_iterator itr = filterList.begin();
	if(itr != filterList.end())
	{
		applyFilter(inR, outR, width, height, filterMap[*itr]);
		applyFilter(inG, outG, width, height, filterMap[*itr]);
		applyFilter(inB, outB, width, height, filterMap[*itr]);
		itr++;
		while(itr != filterList.end())
		{
			char **aux;
			aux = inR; inR = outR; outR = aux;
			aux = inG; inG = outG; outG = aux;
			aux = inB; inB = outB; outB = aux;

			applyFilter(inR, outR, width, height, filterMap[*itr]);
			applyFilter(inG, outG, width, height, filterMap[*itr]);
			applyFilter(inB, outB, width, height, filterMap[*itr]);
			itr++;
		}
	}
	else
	{
		for(int i = 1; i <= height; i++)
		{
			for(int j = 1; j <= width; j++)
			{
				outR[i][j] = inR[i][j];
				outG[i][j] = inG[i][j];
				outB[i][j] = inB[i][j];
			}
		}
	}
	deleteByteMatrix(inR, width + 2, height + 2);
	deleteByteMatrix(inG, width + 2, height + 2);
	deleteByteMatrix(inB, width + 2, height + 2);
	return 0;
}

int initMain(int argc, char *argv[])
{
	if(argc < 3)
	{
		printf("Too few args!\n");
		return -1;
	}

	for(int i = 3; i < argc; i++)
	{
		filterList.push_back(argv[i]);
	}

	fin = fopen(argv[1], "r");
	if(fin == NULL)
	{
		printf("File to read from cannot be opened!\n");
		return -1;
	}

	fout = fopen(argv[2], "w");
	if(fout == NULL)
	{
		printf("File to write to cannot be created!\n");
		fclose(fin);
		return -1;
	}

	filterMap["smooth"] = filter_s{{{(float)1/9, (float)1/9, (float)1/9},
									{(float)1/9, (float)1/9, (float)1/9},
									{(float)1/9, (float)1/9, (float)1/9}}};

	filterMap["blur"] = filter_s{{{(float)1/16, (float)2/16, (float)1/16},
								  {(float)2/16, (float)4/16, (float)2/16},
								  {(float)1/16, (float)2/16, (float)1/16}}};

	filterMap["sharpen"] = filter_s{{{(float)0, (float)-2/3, (float)0},
									 {(float)-2/3, (float)11/3, (float)-2/3},
									 {(float)0, (float)-2/3, (float)0}}};

	filterMap["mean"] = filter_s{{{(float)-1, (float)-1, (float)-1},
								  {(float)-1, (float)9, (float)-1},
								  {(float)-1, (float)-1, (float)-1}}};

	filterMap["emboss"] = filter_s{{{(float)0, (float)1, (float)0},
									{(float)0, (float)0, (float)0},
									{(float)0, (float)-1, (float)0}}};


	for(auto &filter : filterList)
	{
		if(filterMap.find(filter.c_str()) == filterMap.end())
		{
			printf("Filter \"%s\" doesn't exist!\n", filter.c_str());
			return -1;
		}
	}

	return 0;
}

int initAux(int argc, char *argv[])
{

	filterMap["smooth"] = filter_s{{{(float)1/9, (float)1/9, (float)1/9},
									{(float)1/9, (float)1/9, (float)1/9},
									{(float)1/9, (float)1/9, (float)1/9}}};

	filterMap["blur"] = filter_s{{{(float)1/16, (float)2/16, (float)1/16},
								  {(float)2/16, (float)4/16, (float)2/16},
								  {(float)1/16, (float)2/16, (float)1/16}}};

	filterMap["sharpen"] = filter_s{{{(float)0, (float)-2/3, (float)0},
									 {(float)-2/3, (float)11/3, (float)-2/3},
									 {(float)0, (float)-2/3, (float)0}}};

	filterMap["mean"] = filter_s{{{(float)-1, (float)-1, (float)-1},
								  {(float)-1, (float)9, (float)-1},
								  {(float)-1, (float)-1, (float)-1}}};

	filterMap["emboss"] = filter_s{{{(float)0, (float)1, (float)0},
									{(float)0, (float)0, (float)0},
									{(float)0, (float)-1, (float)0}}};

	return 0;
}

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rankpr);
	MPI_Comm_size(MPI_COMM_WORLD, &allnProcesses);

	nProcesses = allnProcesses - 1;
	
	if(rankpr == 0) //main worker
	{

		if(initMain(argc, argv) != 0)
			return -1;

		fscanf(fin, "%s\n", Ptype);
		fscanf(fin, "%[^\n]\n", msg);
		fscanf(fin, "%d %d\n", &width, &height);
		fscanf(fin, "%d\n", &maxval);


		//compute chunk size for each worker (image height in [1; height])
		if(nProcesses >= 1)
		{
			Prow_start[1] = 1;
			Prow_end[1] = height / nProcesses;
			for(int i = 2; i <= nProcesses; i++)
			{
				Prow_end[i] = height / nProcesses + Prow_end[i - 1];
				Prow_start[i] = Prow_end[i - 1] + 1;
			}
			Prow_end[nProcesses] = height;

			//send initial information to workers
			for(int i = 1; i <= nProcesses; i++)
			{
				MPI_Send(&width, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				int heightChunk = Prow_end[i] - Prow_start[i] + 1;
				MPI_Send(&heightChunk, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				MPI_Send(&maxval, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			}
		}
		int err;

		if(Ptype[1] == '5')
		{
			err = p5routine(photoBW, photoBWrez);
			if(err != 0)
			{
				return -1;
			}
			fprintf(fout, "%s\n", Ptype);
			fprintf(fout, "%s\n", msg);
			fprintf(fout, "%d %d\n", width, height);
			fprintf(fout, "%d\n", maxval);
			for(int i = 1; i <= height; i++)
			{
				for(int j = 1; j <= width; j++)
				{
					fprintf(fout, "%c", photoBWrez[i][j]);
				}
			}
			fclose(fout);
			fclose(fin);
			deleteByteMatrix(photoBWrez, width + 2, height + 2);
		}
		else if(Ptype[1] == '6')
		{
			err = p6routine(photoR, photoRrez, photoG, photoGrez, photoB, photoBrez);
			if(err != 0)
			{
				return -1;
			}
			fprintf(fout, "%s\n", Ptype);
			fprintf(fout, "%s\n", msg);
			fprintf(fout, "%d %d\n", width, height);
			fprintf(fout, "%d\n", maxval);
			for(int i = 1; i <= height; i++)
			{
				for(int j = 1; j <= width; j++)
				{
					fprintf(fout, "%c%c%c", photoRrez[i][j], photoGrez[i][j], photoBrez[i][j]);
				}
			}
			fclose(fout);
			fclose(fin);
			deleteByteMatrix(photoRrez, width + 2, height + 2);
			deleteByteMatrix(photoGrez, width + 2, height + 2);
			deleteByteMatrix(photoBrez, width + 2, height + 2);
		}
		else
		{
			printf("Unknown format!\n");
			return -1;
		}
	}


	else //auxiliary workers
	{
		if(initAux(argc, argv) != 0)
		return -1;

		MPI_Status status;
		MPI_Recv(&width, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&height, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&maxval, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		workerComputeParallelPhoto();
	}

	MPI_Finalize();
	return 0;
}
