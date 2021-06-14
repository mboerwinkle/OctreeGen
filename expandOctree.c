#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "octreeOps.h"

void expandOctreeRec(oct* t, oct* ref){
	oct** c = t->child;
	for(int cIdx = 0; cIdx < 8; cIdx++){
		if(c[cIdx])
			expandOctreeRec(c[cIdx], ref);
	}
}
oct* expandOctree(oct* t){
	puts("called expandOctree");
	if(t == NULL) return NULL;
	oct* ret = duplicateOctree(t);
	expandOctreeRec(ret, t);
	/*
	if(t->full) return ret;
	ret = malloc(sizeof(oct));
	ret->mag = t->mag;
	ret->full = 0;
	memcpy(ret->corner, t->corner, sizeof(int)*3);
	for(int cIdx = 0; cIdx < 8; cIdx++){
		if(t->child[cIdx] == NULL){
			ret->child[cIdx] = malloc(sizeof(oct));
			ret->child[cIdx]->mag = ret->mag-1;
			ret->child[cIdx]->full = 1;
			memset(ret->child[cIdx]->child, 0, 8*sizeof(oct*));
			getCubeCorner(cIdx, ret, ret->child[cIdx]);
		}else{
			ret->child[cIdx] = invertOctree(t->child[cIdx]);
		}
	}*/
	return ret;
}
