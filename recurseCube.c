#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "octreeOps.h"

extern int cubeTriangleIntersect(double* center, double sideLen, facet triangle);
void getCorner(int idx, int x1, int y1, int z1, int x2, int y2, int z2, int* write);
int cubeExists(model* target, oct* currentCube);


//oct* newOct = NULL;//this is freed by generateOctree
extern oct* newOct;
int recurseCube(model* target, oct* currentCube){//returns 1 if full, or 0 if incomplete 
/*	int c[3];//corner and shallowCorner combined describe all possible positions for the child corners. Every dimension of shallowCorner is greater than corner.
	memcpy(c, currentCube->corner, 3*sizeof(int));
	int sc[3];
	memcpy(sc, currentCube->corner, 3*sizeof(int));
	for(int dim = 0; dim < 3; dim++){
		sc[dim]+=1<<(currentCube->mag-1);
	}*/
	int fullChildCount = 0;//if we have 8 full children, we are full too
	for(int cIdx = 0; cIdx < 8; cIdx++){
		if(newOct == NULL){//newOct exists to save on allocs and frees.
			newOct = malloc(sizeof(oct));
			newOct->full = 0;
			memset(newOct->child, 0, 8*sizeof(oct*)); 
		}
	//	getCorner(cIdx, c[0], c[1], c[2], sc[0], sc[1], sc[2], newOct->corner);
		getCubeCorner(cIdx, currentCube, newOct);
		newOct->mag = currentCube->mag-1;
		if(cubeExists(target, newOct)){
			currentCube->child[cIdx] = newOct;
			newOct = NULL;
			if(currentCube->child[cIdx]->mag != 0){
				fullChildCount+=recurseCube(target, currentCube->child[cIdx]);
			}else{
				fullChildCount++;
				currentCube->child[cIdx]->full = 1;//all mag 0 cubes are full.
			}
		}
	}
	if(fullChildCount == 8){
		for(int cIdx = 0; cIdx < 8; cIdx++){//if it's full free its children and set to full
			freeOctree(currentCube->child[cIdx]);
		}
		memset(currentCube->child, 0, 8*sizeof(oct*));
		currentCube->full = 1;
	}
	return currentCube->full;
}
void getCubeCorner(int idx, oct* parent, oct* target){
	int c[3];
	memcpy(c, parent->corner, 3*sizeof(int));
	int sc[3];
	memcpy(sc, parent->corner, 3*sizeof(int));
	for(int dim = 0; dim < 3; dim++){
		sc[dim]+=1<<(parent->mag-1);
	}
	int* write = target->corner;
	write[0] = (idx < 4)?c[0]:sc[0];
	write[1] = (idx < 2 || (idx >= 4 && idx < 6))?c[1]:sc[1];
	write[2] = (idx%2 == 0)?c[2]:sc[2];
}

int cubeExists(model* target, oct* currentCube){
	double center[3] = {resolution*currentCube->corner[0], resolution*currentCube->corner[1], resolution*currentCube->corner[2]};
	for(int dim = 0; dim < 3; dim++){
		center[dim]+=resolution*(1<<(currentCube->mag))*0.5;//FIXME store sidelength as arbitrary precision fixed point describing side length instead
	}
	double sideLen = resolution*(1<<(currentCube->mag));
//	printf("testing cube %lf %lf %lf sidelen %lf magnitude %d\n", center[0], center[1], center[2], sideLen, currentCube->mag);
	for(int tFacet = 0; tFacet < target->facetCount; tFacet++){
		if(cubeTriangleIntersect(center, sideLen, target->facets[tFacet])){//FIXME do not recalculate cube faces each time
//			puts("exists!");
//			getchar();
			return 1;
		}
	}
//	puts("does not exist");
//	getchar();
	return 0;
}
