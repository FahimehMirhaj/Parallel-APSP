#include <stdio.h>
#include <time.h>
#include <iostream>
#include "omp.h"
using namespace std;

#define min(a, b) (a < b ? a : b)
#define maxVertices 8192

/* Input Format: Graph is directed and weighted. First two integers must be number of vertices and edges 
 *    which must be followed by pairs of vertices which has an edge between them. 
 */

int dist[maxVertices][maxVertices];
int vertices;


int main(int argc, char *argv[])
{      
	vertices = maxVertices;
	
	omp_set_dynamic(0);
	omp_set_num_threads(omp_get_num_procs());


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
	
	clock_t tStart = clock();
	
	/*
	parallel for
		parallel for
			for
				==> 6805290.000000
	*/
	#pragma omp parallel for
	for(int via=0;via<vertices;via++)
	{
		
		#pragma omp parallel for
		for(int from = 0; from < vertices; from++)
        {
            for(int to = 0; to < vertices; to++)
            {
                if(from!=to && from!=via && to!=via)
				{
					dist[from][to] = min(dist[from][to],dist[from][via]+dist[via][to]);
				}
                        
            }
        }	
		
    }

	clock_t tEnd = clock();

	printf("OMP Parallel version is done!!\n");

    double msecs = ((double) (tEnd - tStart)) * 1000.0 / CLOCKS_PER_SEC;
    printf("\nOMP Time taken: %lf\n", msecs);
    return 0;
}

