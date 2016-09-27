#include <stdio.h>
#include <time.h>
#include <iostream>
#include <algorithm>
#include <cilk/cilk.h>
using namespace std;

#define maxVertices 16
#define m 4

int dist[maxVertices][maxVertices];
int dist2[maxVertices][maxVertices];
int vertices;

void FloydWarshall_iter(int size, int kk, int ii, int jj)
{
        int k;
        for (k = kk; k < kk+m; ++k) {
                int i;
                for (i = ii; i < ii+m; ++i) {
                        int j;
                        for (j = jj; j < jj+m; ++j) {
                                dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
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
                cilk_spawn FloydWarshall_recurC(n, k, i, j, size);
                cilk_spawn FloydWarshall_recurC(n, k, i+size, j, size);
                cilk_sync;
        		cilk_spawn FloydWarshall_recurC(n, k, i, j+size, size);
                cilk_spawn FloydWarshall_recurC(n, k, i+size, j+size, size);
                cilk_sync;
		        cilk_spawn FloydWarshall_recurC(n, k+size, i, j+size, size);
                cilk_spawn FloydWarshall_recurC(n, k+size, i+size, j+size, size);
                cilk_sync;
		        cilk_spawn FloydWarshall_recurC(n, k+size, i, j, size);
                cilk_spawn FloydWarshall_recurC(n, k+size, i+size, j, size);
                cilk_sync;
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
                cilk_spawn FloydWarshall_recurB(n, k, i, j, size);
                cilk_spawn FloydWarshall_recurB(n, k, i, j+size, size);
                cilk_sync;
		        cilk_spawn FloydWarshall_recurB(n, k, i+size, j, size);
                cilk_spawn FloydWarshall_recurB(n, k, i+size, j+size, size);
                cilk_sync;
		        cilk_spawn FloydWarshall_recurB(n, k+size, i+size, j, size);
                cilk_spawn FloydWarshall_recurB(n, k+size, i+size, j+size, size);
                cilk_sync;
		        cilk_spawn FloydWarshall_recurB(n, k+size, i, j, size);
                cilk_spawn FloydWarshall_recurB(n, k+size, i, j+size, size);
                cilk_sync;
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
                cilk_spawn FloydWarshall_recurB(n, k, i, j+size, size);
                cilk_spawn FloydWarshall_recurC(n, k, i+size, j, size);
                cilk_sync;
		        FloydWarshall_recurA(n, k, i+size, j+size, size);
                FloydWarshall_recurA(n, k+size, i+size, j+size, size);
                cilk_spawn FloydWarshall_recurB(n, k+size, i+size, j, size);
                cilk_spawn FloydWarshall_recurC(n, k+size, i, j+size, size);
                cilk_sync;
		        FloydWarshall_recurA(n, k+size, i, j, size);
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
	for(int i = 0 ; i < vertices ; i++ )
	{
        	for(int j = 0 ; j< vertices; j++ )
           	{
                	if( i == j )
				dist[i][j] = 0;
			else
        		dist[i][j] = i+j;
		}
	}

	/*iterativeFW(vertices);
        cout << "Iterative is done !!" << endl;
	*/
	clock_t tStart = clock();
	FloydWarshall_recurA(vertices, 0, 0, 0, vertices);
	clock_t tEnd = clock();
	cout << "Recursive FW computation completed successfully !!" << endl;
	/*for(int i = 0 ; i < vertices; i++ )
    {
        for(int j = 0 ; j< vertices ;j++ )
            if(dist[i][j] != dist2[i][j])
            {
                printf("Mismatch!\n");
            }
    }*/

	double msecs = ((double) (tEnd - tStart)) * 1000.0 / CLOCKS_PER_SEC;
	printf("Time taken: %lf\n", msecs);
	return 0;
}
