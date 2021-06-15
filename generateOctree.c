#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "octreeOps.h"

double resolution = 1;
char expandflag = 0;
int magnitude = 0;//magnitude 0 is a single cube centered on the origin. This never exists because we require a 1 block buffer around all objects, which cannot exist with a single block (unless the model is empty - autocheck FIXME)
extern oct* newOct;//used in recurseCube
extern void recurseCube(model* target, oct* currentCube);
extern int cubeExists(model* target, oct* currentCube);

model generateOctree(model target, double modelSize){//based on cubealgo.txt
	//initialize values
	target.myTree = NULL;
	float* min = target.min;
	float* max = target.max;
	for(int dim = 0; dim < 3; dim++){
		min[dim] = 0;
		max[dim] = 0;
	}
	for(int fIdx = 0; fIdx < target.facetCount; fIdx++){
		facet* f = &(target.facets[fIdx]);
		for(int dim = 0; dim < 3; dim++){
			if(f->p1[dim] < min[dim]) min[dim] = f->p1[dim];
			if(f->p1[dim] > max[dim]) max[dim] = f->p1[dim];
			if(f->p2[dim] < min[dim]) min[dim] = f->p2[dim];
			if(f->p2[dim] > max[dim]) max[dim] = f->p2[dim];
			if(f->p3[dim] < min[dim]) min[dim] = f->p3[dim];
			if(f->p3[dim] > max[dim]) max[dim] = f->p3[dim];
		}
	}
	float maxdiff = 0;
	for(int dim = 0; dim < 3; dim++){
		if(max[dim]-min[dim] > maxdiff) maxdiff = max[dim]-min[dim];
	}
	for(int dim = 0; dim < 3; dim++){
		min[dim] *= modelSize/maxdiff;
		max[dim] *= modelSize/maxdiff;
	}
	for(int fIdx = 0; fIdx < target.facetCount; fIdx++){//Fixme this should be doable as a greatest multiplier like resolution is done
		facet* f = &(target.facets[fIdx]);
		for(int dim = 0; dim < 3; dim++){
			f->p1[dim] *= modelSize/maxdiff;
			f->p2[dim] *= modelSize/maxdiff;
			f->p3[dim] *= modelSize/maxdiff;
		}
	}
	double greatest = 0;
	for(int dim = 0; dim < 3; dim++){
		if(-min[dim] > greatest) greatest = -min[dim];
		if(max[dim] > greatest) greatest = max[dim];
	}
	fprintf(stderr, "min: %f %f %f\nmax: %f %f %f\n", min[0],min[1],min[2], max[0],max[1],max[2]);
	fprintf(stderr, "greatest: %lf\n", greatest);
	greatest/=resolution;//greatest divided by resolution so we can ignore resolution from now on
	while(greatest*2+2 > 1<<magnitude){//multiply greatest by 2 because maximum magnitude cube is centered on the origin. +2 for the buffer on each side.
		magnitude++;
	}
	if(expandflag){
		fprintf(stderr, "increasing magnitude for safe expansion\n");
		magnitude += 2;
	}
	fprintf(stderr, "magnitude: %d\n", magnitude);
	int sideLength = 1<<magnitude;
	if(sideLength >= 2097100){//just ~(2^63)^(1/3)
		fprintf(stderr, "Volume Overflows long int\n");
	}
	long int volume = sideLength*sideLength*sideLength;
	fprintf(stderr, "sideLength: %d\nvolume: %ld\n", sideLength, volume);
	//find boundary
	target.myTree = malloc(sizeof(oct));
	target.myTree->mag = magnitude;
	target.myTree->full = 0;
	memset(target.myTree->child, 0, 8*sizeof(oct*));
	for(int dim = 0; dim < 3; dim++){
		target.myTree->corner[dim] = -sideLength/2;
	}
	fprintf(stderr, "corner %d %d %d\n", target.myTree->corner[0], target.myTree->corner[1], target.myTree->corner[2]);  
	int writeFrequency = target.facetCount/10;
	for(int tIdx = 0; tIdx < target.facetCount; tIdx++){
		if(writeFrequency == 0 || tIdx%writeFrequency == 0){
			fprintf(stderr, "facet %d of %d\n", tIdx, target.facetCount);
		}
		addTriangle(target.myTree, &(target.facets[tIdx]), resolution);
	}
	free(newOct);
/*	if(cubeExists(&target, target.myTree)){
		recurseCube(&target, target.myTree);
		free(newOct);
	}else{
		free(target.myTree);
		target.myTree = NULL;
	}*/
	fprintf(stderr, "Done finding border\n");
	return target;
}
