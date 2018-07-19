#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "octreeOps.h"

double resolution = 1;
int magnitude = 0;//magnitude 0 is a single cube centered on the origin. This never exists because we require a 1 block buffer around all objects, which cannot exist with a single block (unless the model is empty - autocheck FIXME)
extern oct* newOct;//used in recurseCube
extern void recurseCube(model* target, oct* currentCube);
extern int cubeExists(model* target, oct* currentCube);

model generateOctree(model target){//based on cubealgo.txt
	//initialize values
	target.myTree = NULL;
	float* min = target.min;
	float* max = target.max;
	for(int dim = 0; dim < 3; dim++){
		min[dim] = 0;
		max[dim] = 0;
	}
	for(int fIdx = 0; fIdx < target.facetCount; fIdx++){
		float* p1 = target.facets[fIdx].p1;
		float* p2 = target.facets[fIdx].p2;
		float* p3 = target.facets[fIdx].p3;
		for(int dim = 0; dim < 3; dim++){
			if(p1[dim] < min[dim]) min[dim] = p1[dim];
			if(p1[dim] > max[dim]) max[dim] = p1[dim];
			if(p2[dim] < min[dim]) min[dim] = p2[dim];
			if(p2[dim] > max[dim]) max[dim] = p2[dim];
			if(p3[dim] < min[dim]) min[dim] = p3[dim];
			if(p3[dim] > max[dim]) max[dim] = p3[dim];
		}
	}
	printf("min: %f %f %f\nmax: %f %f %f\n", min[0],min[1],min[2], max[0],max[1],max[2]);
	double greatest = 0;
	for(int dim = 0; dim < 3; dim++){
		if(-min[dim] > greatest) greatest = -min[dim];
		if(max[dim] > greatest) greatest = max[dim];
	}
	printf("greatest: %lf\n", greatest);
	greatest/=resolution;//greatest divided by resolution so we can ignore resolution from now on
	while(greatest*2+2 > 1<<magnitude){//multiply greatest by 2 because maximum magnitude cube is centered on the origin. +2 for the buffer on each side.
		magnitude++;
	}
	printf("magnitude: %d\n", magnitude);
	int sideLength = 1<<magnitude;
	if(sideLength >= 2097100){//just ~(2^63)^(1/3)
		puts("Volume Overflows long int");
	}
	long int volume = sideLength*sideLength*sideLength;
	printf("sideLength: %d\nvolume: %ld\n", sideLength, volume);
	//find boundary
	target.myTree = malloc(sizeof(oct));
	target.myTree->mag = magnitude;
	target.myTree->full = 0;
	memset(target.myTree->child, 0, 8*sizeof(oct*));
	for(int dim = 0; dim < 3; dim++){
		target.myTree->corner[dim] = -sideLength/2;
	}
	printf("corner %d %d %d\n", target.myTree->corner[0], target.myTree->corner[1], target.myTree->corner[2]);  
	int writeFrequency = target.facetCount/10;
	for(int tIdx = 0; tIdx < target.facetCount; tIdx++){
		if(writeFrequency == 0 || tIdx%writeFrequency == 0){
			printf("facet %d of %d\n", tIdx, target.facetCount);
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
	puts("Done finding border");
	return target;
}
