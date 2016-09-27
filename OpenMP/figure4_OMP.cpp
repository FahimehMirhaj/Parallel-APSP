// to compile: g++ -fopenmp figure4_OMP.cpp -o figure4_OMP
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <limits.h>
#include <algorithm>
#include "omp.h"
using namespace std;

#define maxVertices 512
#define m 64


int dist[maxVertices][maxVertices];
int dist2[maxVertices][maxVertices];
int vertices;


void iterativeFW(int size) {
    int k;
    for (k = 0; k < size; ++k) {
        int i;
        for (i = 0; i < size; ++i) {
            int j;
            for (j = 0; j < size; ++j) {
                dist2[i][j] = min(dist2[i][j], dist2[i][k] + dist2[k][j]);
            }
        }
    }
}

void FloydWarshall_iter(int n, int k, int i, int j)
{
        
	for(int p; p<k+m; p++)
	{
		for(int q; q<i+m; q++)
        	{
                	for(int r; r<j+m; r++)
                	{
				        dist[q][r] = min(dist[q][r], dist[q][p]+ dist[p][r]);
                	}
        	}
   	}
}


void FloydWarshall_recurC(int n, int k, int i, int j, int size)
{
        if (size == m)
        {
                FloydWarshall_iter(n, k, i, j);
        }
        else
        {
            size /= 2;
            #pragma omp task firstprivate(n, k, i, j, size)
            {
                FloydWarshall_recurC(n, k, i, j, size);
            }

            #pragma omp task firstprivate(n, k, i, j, size)
            {
                FloydWarshall_recurC(n, k, i+size, j, size);
            }

            #pragma omp taskwait

	        #pragma omp task firstprivate(n, k, i, j, size)
            {
                FloydWarshall_recurC(n, k, i, j+size, size);
            }

            #pragma omp task firstprivate(n, k, i, j, size)
            { 
                FloydWarshall_recurC(n, k, i+size, j+size, size);
            }

            #pragma omp taskwait
	        #pragma omp task firstprivate(n, k, i, j, size)
            {
                FloydWarshall_recurC(n, k+size, i, j+size, size);
            }
            #pragma omp task firstprivate(n, k, i, j, size)
            {
                FloydWarshall_recurC(n, k+size, i+size, j+size, size);
            }
            #pragma omp taskwait

	        #pragma omp task firstprivate(n, k, i, j, size)
            {
                FloydWarshall_recurC(n, k+size, i, j, size);
            }
            #pragma omp task firstprivate(n, k, i, j, size)
            {
                FloydWarshall_recurC(n, k+size, i+size, j, size);
            }

            #pragma omp taskwait
        }
}


void FloydWarshall_recurB(int n, int k, int i, int j, int size)
{
        if (size == m)
        {
                FloydWarshall_iter(n, k, i, j);
        }
        else
        {
            size /= 2;
            #pragma omp task firstprivate(n, k, i, j, size)
            {
                FloydWarshall_recurB(n, k, i, j, size);
            }

            #pragma omp task firstprivate(n, k, i, j, size)
            {
                FloydWarshall_recurB(n, k, i, j+size, size);
            }

            #pragma omp taskwait

            #pragma omp task firstprivate(n, k, i, j, size)
            {
                FloydWarshall_recurB(n, k, i+size, j, size);
            }
	
            #pragma omp task firstprivate(n, k, i, j, size)
            {
                FloydWarshall_recurB(n, k, i+size, j+size, size);
            }
            #pragma omp taskwait

            #pragma omp task firstprivate(n, k, i, j, size)
            {
               FloydWarshall_recurB(n, k+size, i+size, j, size); 
            }
            #pragma omp task firstprivate(n, k, i, j, size)
            {
               FloydWarshall_recurB(n, k+size, i+size, j+size, size); 
            }

            #pragma omp taskwait
            #pragma omp task firstprivate(n, k, i, j, size)
            {
                FloydWarshall_recurB(n, k+size, i, j, size);
            }

            #pragma omp task firstprivate(n, k, i, j, size)
            {
                FloydWarshall_recurB(n, k+size, i, j+size, size);
            }
            
            #pragma omp taskwait
        }
}


void FloydWarshall_recurA(int n, int k, int i, int j, int size)
{
        if (size == m)
        {
                FloydWarshall_iter(n, k, i, j);
        }
        else
        {
            size /= 2;
        
            FloydWarshall_recurA(n, k, i, j, size);    
            
            #pragma omp task firstprivate(n, k, i, j, size)
            {
                FloydWarshall_recurB(n, k, i, j+size, size);
            }

            #pragma omp task firstprivate(n, k, i, j, size)
            {
                FloydWarshall_recurC(n, k, i+size, j, size);    
            }

            #pragma omp taskwait

    		FloydWarshall_recurA(n, k, i+size, j+size, size);
            FloydWarshall_recurA(n, k+size, i+size, j+size, size);

            #pragma omp task firstprivate(n, k, i, j, size)
            {
                FloydWarshall_recurB(n, k+size, i+size, j, size);
            }


            #pragma omp task firstprivate(n, k, i, j, size)
            {
                FloydWarshall_recurC(n, k+size, i, j+size, size);
            }

            #pragma omp taskwait

	        FloydWarshall_recurA(n, k+size, i, j, size);
        }
}


int main(int argc, char *argv[])
{      
	vertices = maxVertices;
	
	for(int i = 0 ; i < vertices ; i++ )
    {
            for(int j = 0 ; j< vertices; j++ )
            {
                    if( i == j )
            {
                dist[i][j] = 0;
                dist2[i][j] = 0;
            }
            else
            {
                dist[i][j] = i+j;
                dist2[i][j] = i+j;
            }
        }
    }
	
    iterativeFW(vertices);
    cout << "Iterative is done !!" << endl;
    
    
	clock_t tStart = clock();
    #pragma omp parallel
    {
        #pragma omp single
        {
            FloydWarshall_recurA(vertices, 0, 0, 0, vertices);
        }
    }
	clock_t tEnd = clock();
	
    for(int i = 0 ; i < vertices; i++ )
    {
        for(int j = 0 ; j< vertices ;j++ )
            if(dist[i][j] != dist2[i][j])
            {
                printf("Mismatch!\n");
            }
    }

	double msecs = ((double) (tEnd - tStart)) * 1000.0 / CLOCKS_PER_SEC;
	printf("\nTime taken: %lf\n", msecs);
        return 0;
}
