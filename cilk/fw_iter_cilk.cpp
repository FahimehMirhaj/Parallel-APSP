#include <stdio.h>
#include <time.h>
#include <algorithm>
#include <iostream>
#include <cilk/cilk.h>
using namespace std;

#define maxVertices 8192

int dist[maxVertices][maxVertices];
int vertices;

void FloydWarshall(int vertices)
{

	for(int via=0;via<vertices;via++)
	{
		cilk_for(int from=0;from<vertices;from++)
        	{
            		cilk_for(int to=0;to<vertices;to++)
            		{
                		if(from!=to && from!=via && to!=via)
				{
					dist[from][to] = min(dist[from][to],dist[from][via]+dist[via][to]);
				}

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

	clock_t tStart = clock();
	FloydWarshall(vertices);
	clock_t tEnd = clock();
    	double msecs = ((double) (tEnd - tStart)) * 1000.0 / CLOCKS_PER_SEC;
    	printf("Time taken: %lf\n", msecs);
    	return 0;
}