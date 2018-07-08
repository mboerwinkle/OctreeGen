#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "octreeOps.h"

extern int cubeTriangleIntersect(double* center, double sideLen, facet triangle);
extern void getCubeCorner(int idx, oct* parent, oct* target);
oct* newOct = NULL;
void addTriangle(oct* t, facet* tri, double res){//very similar to recurseCube. Just a little different (Operates on a single triangle at a time, instead of the entire group of triangles
	int fullChildCount = 0;
	for(int cIdx = 0; cIdx < 8; cIdx++){
		if(t->child[cIdx] != NULL){
			if(t->child[cIdx]->full){//if this child is already full, then it doesn't matter if it intersects or not.
				fullChildCount++;
				continue;
			}
		}else{
			if(newOct == NULL){
				newOct = malloc(sizeof(oct));
				newOct->full = 0;
				memset(newOct->child, 0, 8*sizeof(oct*));
			}
			getCubeCorner(cIdx, t, newOct);
			newOct->mag = t->mag-1;
			t->child[cIdx] = newOct;
		}
		//FIXME make cubeTriangleIntersect and generateCubeFacets operate from the corner instead of the center.
		double center[3] = {res*t->child[cIdx]->corner[0], res*t->child[cIdx]->corner[1], res*t->child[cIdx]->corner[2]};
		for(int dim = 0; dim < 3; dim++){
			center[dim]+=res*(1<<(t->child[cIdx]->mag))*0.5;
		}
		double sideLen = res*(1<<(t->child[cIdx]->mag));
		if(cubeTriangleIntersect(center, sideLen, *tri)){//FIXME make cubeTriangleIntersect operate on facet pointers instead of facets
			if(newOct == t->child[cIdx]){
				newOct = NULL;
			}
			if(t->child[cIdx]->mag != 0){
				addTriangle(t->child[cIdx], tri, res);
			}else{
				t->child[cIdx]->full = 1;
			}
			if(t->child[cIdx]->full) fullChildCount++;
		}else{
			if(newOct == t->child[cIdx]){//this child doesn't exist yet.
				t->child[cIdx] = NULL;
			}
		}
	}
	if(fullChildCount == 8){
		for(int cIdx = 0; cIdx < 8; cIdx++){
			freeOctree(t->child[cIdx]);
		}
		memset(t->child, 0, 8*sizeof(oct*));
		t->full = 1;
	}
}
