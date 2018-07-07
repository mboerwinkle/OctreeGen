#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
void createCubeTriangles(double* center, double sideLen, facet* facetArray){//FIXME use integer offsets to avoid fp problems on large models (Do after octree working conversion)
	int facetIdx = 0;
	double len = sideLen/2;
	for(int dim = 0; dim < 3; dim++){//each direction
		for(int mult = -1; mult <= 2; mult+=2){//positive and negative face
			facetArray[facetIdx].p1[dim] = len*mult;//FIXME code duplication
			facetArray[facetIdx].p2[dim] = len*mult;//we set all of the dim faces to their plane
			facetArray[facetIdx].p3[dim] = len*mult;
			facetArray[facetIdx+1].p1[dim] = len*mult;
			facetArray[facetIdx+1].p2[dim] = len*mult;
			facetArray[facetIdx+1].p3[dim] = len*mult;
			int otherDim = 1;
			facetArray[facetIdx].p1[(dim+otherDim)%3] = len;
			facetArray[facetIdx].p2[(dim+otherDim)%3] = len;
			facetArray[facetIdx].p3[(dim+otherDim)%3] = -len;
			facetArray[facetIdx+1].p1[(dim+otherDim)%3] = -len;
			facetArray[facetIdx+1].p2[(dim+otherDim)%3] = len;
			facetArray[facetIdx+1].p3[(dim+otherDim)%3] = -len;
			otherDim = 2;
			facetArray[facetIdx].p1[(dim+otherDim)%3] = -len;
			facetArray[facetIdx].p2[(dim+otherDim)%3] = len;
			facetArray[facetIdx].p3[(dim+otherDim)%3] = -len;
			facetArray[facetIdx+1].p1[(dim+otherDim)%3] = len;
			facetArray[facetIdx+1].p2[(dim+otherDim)%3] = len;
			facetArray[facetIdx+1].p3[(dim+otherDim)%3] = -len;
			facetIdx+=2;
		}
		//The above code creates a cube centered on the origin. The below code translates that cube.
	}
	for(int tIdx = 0; tIdx < 12; tIdx++){
		facetArray[tIdx].attr = 0;
		for(int dim = 0; dim < 3; dim++){
			facetArray[tIdx].vec[dim] = 0;
			facetArray[tIdx].p1[dim]+= center[dim];
			facetArray[tIdx].p2[dim]+= center[dim];
			facetArray[tIdx].p3[dim]+= center[dim];
		}
	}
}
