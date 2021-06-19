#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "octreeOps.h"

extern int cubeTriangleIntersect(double* center, double sideLen, facet triangle);
int addTriangle(subtree* t, facet* tri, double res){
	int fullChildCount = 0;
	subtree child = childSubtree(t, 0);
	for(int cIdx = 0; cIdx < 8; cIdx++){
		if(cIdx > 0) child = siblingSubtree(&child, cIdx-1);
		char cstatus = getStatus(&child);
		//if this child is already full, then it doesn't matter if it intersects or not.
		if(cstatus == 'F'){
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
				if(cstatus == 'E') setStatus(&child, 'P', 'E');
				if(addTriangle(&child, tri, res)){
					fullChildCount++;
				}
			}else{
				setStatus(&child, 'F', 0);
				fullChildCount++;
			}
		}
	}
	if(fullChildCount == 8){
		setStatus(t, 'F', 0);
		return 1;
	}
	return 0;
}
