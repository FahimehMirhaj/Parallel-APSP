// to compile: g++ -fopenmp figure2_OMP.cpp -o figure2_OMP
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <limits.h>
#include <algorithm>
#include "omp.h"
using namespace std;

//#define INF 2147483647
#define maxVertices 512
#define m 64

/* Input Format: Graph is directed and weighted. First two integers must be number of vertices and edges 
 *    which must be followed by pairs of vertices which has an edge between them. 
 */

int dist[maxVertices][maxVertices];
int dist2[maxVertices][maxVertices];
int vertices;

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


void FloydWarshall_recur1(int n, int k, int i, int j, int size)
{
	if (size == m)
	{
		FloydWarshall_iter(n, k, i, j);
	}
	else
	{
		size /= 2;
        FloydWarshall_recur1(n, k, i, j, size);
        #pragma omp task firstprivate(n, k, i, j, size)
		{
			FloydWarshall_recur1(n, k, i, j+size, size);
		}
        #pragma omp task firstprivate(n, k, i, j, size)
		{
			FloydWarshall_recur1(n, k, i+size, j, size);
		}
        #pragma omp taskwait

		FloydWarshall_recur1(n, k, i+size, j+size, size);
        FloydWarshall_recur1(n, k+size, i+size, j+size, size);
        #pragma omp task firstprivate(n, k, i, j, size)
		{
			FloydWarshall_recur1(n, k+size, i+size, j, size);
		}
        #pragma omp task firstprivate(n, k, i, j, size)
		{
			FloydWarshall_recur1(n, k+size, i, j+size, size);
		}
        #pragma omp taskwait
		FloydWarshall_recur1(n, k+size, i, j, size);
	}
}

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

int main(int argc, char *argv[])
{      
	vertices = maxVertices;
	/*initialize dist between all pairs as infinity*/
	//init(vertices);
        /* vertices represent number of vertices and edges represent number of edges in the graph. */
		
	
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
                FloydWarshall_recur1(vertices, 0, 0, 0, vertices);
            }
    }
	clock_t tEnd = clock();

    cout << "Recursive is done !!" << endl;

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
