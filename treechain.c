#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "structures.h"
#include "octreeOps.h"

treechain* createTreechain(oct* t){
	treechain* ret = malloc(sizeof(treechain));
	ret->t = calloc(t->mag+1, sizeof(subtree));
	ret->t[t->mag] = rootSubtree(t);
	ret->mm = t->mag;
	ret->maxmag = t->mag;
	return ret;
}
void freeTreechain(treechain* t){
	free(t->t);
	free(t);
}
char cornerExistsTC(treechain* t, pt loc, int mag, int* foundMagnitude, pt* foundCorner){
	assert(mag <= t->maxmag);
	int* mm = &(t->mm);
	//get up to a sane working height
	if(mag > *mm) *mm = mag;
	//go up while we are outside of the target
	while(1){
		subtree* curr = &(t->t[*mm]);
		long int sideLen = sidelen(*mm);
		char bad = 0;
		for(int dim = 0; dim < DIM; dim++){
			if(curr->corner.l[dim] > loc.l[dim] || curr->corner.l[dim]+sideLen <= loc.l[dim]){
				if((*mm) == t->maxmag) return -1;
				(*mm)++;
				bad = 1;
				break;
			}
		}
		if(bad) continue;
		break;
	}
	//then go down while we are overlapping the target
	while(1){
		subtree* curr = &(t->t[*mm]);
		char status = getStatus(curr);
		if(status == 'F'){
			if(foundMagnitude != NULL) *foundMagnitude = curr->mag;
			if(foundCorner != NULL) *foundCorner = curr->corner;
			return 2;
		}else if(status == 'E'){
			return 0;
		}else if(curr->mag == mag){
			#ifndef NDEBUG
				for(int idx = 0; idx < DIM; idx++){
					assert(loc.l[idx] == curr->corner.l[idx]);
				}
			#endif
			return 1;
		}
		
		t->t[*mm - 1] = childSubtree(curr, identifyCorner(curr, loc));
		(*mm)--;
	}
}
