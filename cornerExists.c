#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "structures.h"
#include "octreeOps.h"
//-1 invalid
//0 does not exist (empty)
//1 partial (exists, but not full)
//2 exists fully at the specified magnitude
//
//foundMagnitude undefined unless return value is 2 (full)
//foundCorner is populated in the same conditions as foundMagnitude with the corner of whatever cube includes loc
char cornerExistsRec(subtree* t, pt loc, int mag, int* foundMagnitude, pt* foundCorner){
	long int sideLen = sidelen(t->mag);
	for(int dim = 0; dim < DIM; dim++){
		if(t->corner.l[dim] > loc.l[dim] || t->corner.l[dim]+sideLen <= loc.l[dim]){
			return -1;
		}
	}
	char status = getStatus(t);
	if(status == 'F'){
		if(foundMagnitude != NULL) *foundMagnitude = t->mag;
		if(foundCorner != NULL) *foundCorner = t->corner;
		return 2;
	}else if(status == 'E'){
		return 0;
	}else if(t->mag == mag){
		#ifndef NDEBUG
			for(int idx = 0; idx < DIM; idx++){
				assert(loc.l[idx] == t->corner.l[idx]);
			}
		#endif
		return 1;
	}
	subtree child = childSubtree(t, identifyCorner(t, loc));
	return cornerExistsRec(&child, loc, mag, foundMagnitude, foundCorner);
} 
char cornerExists(oct* t, pt corner, int mag, int *foundMagnitude, pt* foundCorner){
	subtree root = rootSubtree(t);
	return cornerExistsRec(&root, corner, mag, foundMagnitude, foundCorner);
}

void addCornerRec(subtree* t, pt loc, int mag){
	char tstatus = getStatus(t);
	if(tstatus == 'F') return;
	if(t->mag == mag){
		#ifndef NDEBUG
			for(int idx = 0; idx < DIM; idx++){
				if(loc.l[idx] != t->corner.l[idx]){
					fprintf(stderr, "Assertion Failed: mag %hd loc (%ld %ld %ld) is not corner (%ld %ld %ld)\n", mag, loc.l[0], loc.l[1], loc.l[2], t->corner.l[0], t->corner.l[1], t->corner.l[2]);
					break;
				}
			}
		#endif
		setStatus(t, 'F', 0);
		return;
	}
	if(tstatus == 'E') setStatus(t, 'P', 'E');
	int cIdx = identifyCorner(t, loc);
	subtree child = childSubtree(t, cIdx);
	addCornerRec(&child, loc, mag);
}
void addCorner(oct* t, pt corner, int mag){
	subtree root = rootSubtree(t);
	addCornerRec(&root, corner, mag);
}

int identifyCorner(subtree* parent, pt internal){
	unsigned long int sideLen2 = sidelen(parent->mag-1);
	int ret = 0;
	for(int dim = 0; dim < DIM; dim++){
		ret += ((internal.l[dim]-parent->corner.l[dim])/sideLen2 > 0)?(1<<(DIM-dim-1)):0;
	}
	return ret;
}
