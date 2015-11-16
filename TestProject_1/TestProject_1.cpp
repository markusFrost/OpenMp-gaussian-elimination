// TestProject_1.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "stdio.h"
#include <iostream>
#include <conio.h>
#include <omp.h>
#include <time.h>
#include <assert.h>
#include <ctime> 
using namespace std;

void randInt( float* a, float* b, int N)
{
	int i;
	for (i = 0; i < N * N; i++)
	{
		a[i] = (int) (rand() / (float)RAND_MAX * 10 );
		if  ( i % N == 0 )
		{
			b[i/N] = (int) (rand() / (float)RAND_MAX * 10 );
		}
	}
		
}

void partialPivot( float* a, float* b, int N, int curpos)
{
	int j, i_max;

	float t;

	i_max = curpos;

	for ( j = curpos + 1; j < N; j++ )
	{
		if (a[j*N+curpos] > a[i_max*N+curpos])
		{
			i_max = j;
		}
	}

	for ( j = 0; j < N; ++j )
	{
		t = a[i_max*N+j];
		a[i_max*N+j] = a[curpos*N+j];
		a[curpos*N+j] = t;
	}

	for ( j = 0; j < N; ++j )
	{
		t = b[i_max];
		b[i_max] = b[curpos];
		b[curpos] = t;
	}
}



int _tmain(int argc, _TCHAR* argv[])
{
	locale::global(locale("rus"));
	int N;
	float * A;
	float* b;
	float* X;

	int i, j, k,x, y, i_max;

	int thread_num = omp_get_max_threads();

	double startTime = 0, elapsedTime = 0;
	double clockZero = 0.0;

	printf("Введите размер матрицы ");
	scanf_s("%d", &N);
	printf("______________________\n");

	A = (float*) malloc(sizeof(float)*N*N); assert(A);
	b = (float*) malloc(sizeof(float)*N); assert(A);
	X = (float*) malloc(sizeof(float)*N); assert(A);

	srand(time(NULL));

	printf ("Исходная матрица\n" );

	randInt(A,b,N);

	for ( x = 0; x < N; x++)
	{
		for ( y = 0; y < N; y++ )
		{
			printf( "%.2f ", A[x + N + y] );
		}
		printf( "| %.2f\n ", b[x] );
	}


	//startTime = clock();
	startTime = omp_get_wtime();
	printf("\nОбратный ход\n");
	for ( k = 0; k < N - 1; k++ )
	{
		partialPivot(A, b, N, k);

#pragma omp parallel default(none) num_threads(thread_num) \
	shared(A,b,N,k) private(i,j)
		{
#pragma omp for schedule(static)
			for ( i = k + 1; i < N; i++){
				for ( j = k; j < N; j++)
					A[i*N+j] = A[i*N+j] - (A[i*N+k]/A[k*N+k]) * A[k*N+j];
				b[i] = b[i] - (A[i*N+k]/A[k*N+k])*b[k];
			}
		}
	}

	//elapsedTime = clock() - startTime;
	elapsedTime = omp_get_wtime();

	printf("\n--------------\n");

	for ( x = 0; x < N; x++ ){
		for ( y = 0; y < N; y++ )
			printf("%.2f ", A[x*N+y] );
		printf("| %.2f\n", b[x] );
	}

	printf("%d threads\n", thread_num);
	printf("Время = %6.4f (s)\n", elapsedTime - startTime );   
	printf("\nРезультат выполнения:\n");

	for ( int index = 0; index < N; index++)
	{
		X[index] = 0;
	}

	for(int m=N-1; m >= 0; m--){
        double sum=0;
        // идем по строке спаво налево, считая сумму корень*коэфицент, до текущего корня
        for(int i=N-1; i > m; i--){
            sum += X[i] * A[m * N + i];//[i]; //x*N+y
        }
        X[m] = (b[m] - sum)/A[m * N + m];//[m][m];
    }

	for ( int index = 0; index < N; index++)
	{
		printf("%.2f\t", X[index] );
	}

	printf("\n");
	
	
     _getch();
	return 0;
}


