#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "octreeOps.h"

extern int cubeTriangleIntersect(double* center, double sideLen, facet triangle);
void addTriangle(subtree* t, facet* tri, double res){
	int fullChildCount = 0;
	subtree child;
	for(int cIdx = 0; cIdx < 8; cIdx++){
		child = childSubtree(t, cIdx);
		//if this child is already full, then it doesn't matter if it intersects or not.
		if(getStatus(&child) == 'F'){
			fullChildCount++;
			continue;
		}
		//FIXME make cubeTriangleIntersect and generateCubeFacets operate from the corner instead of the center.
		double center[3];
		for(int dim = 0; dim < 3; dim++){
			center[dim]=res*((double)sidelen(child.mag)*0.5 + child.corner.l[dim]);
		}
		double sideLen = res*(double)sidelen(child.mag);
		if(cubeTriangleIntersect(center, sideLen, *tri)){
			if(child.mag != 0){
				setStatus(&child, 'P');
				addTriangle(&child, tri, res);
			}else{
				setStatus(&child, 'F');
				fullChildCount++;
			}
		}
	}
	if(fullChildCount == 8){
		setStatus(t, 'F');
	}
}
