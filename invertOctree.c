#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "octreeOps.h"

oct* invertOctree(oct* t){//duplicates all non-full cubes. Replaces nulls with full cubes, and full cubes with nulls
	if(t == NULL) printf("FATAL! invertOctree cannot operate on NULL pointer\n");
	if(t->full) return NULL;
	oct* ret = malloc(sizeof(oct));
	ret->mag = t->mag;
	ret->full = 0;
	memcpy(ret->corner, t->corner, sizeof(int)*3);
	for(int cIdx = 0; cIdx < 8; cIdx++){
		if(t->child[cIdx] == NULL){
			ret->child[cIdx] = malloc(sizeof(oct));
			ret->child[cIdx]->mag = ret->mag-1;
			ret->child[cIdx]->full = 1;
			memset(ret->child[cIdx]->child, 0, 8*sizeof(oct*));
			getCubeCorner(cIdx, ret, ret->child[cIdx]->corner);
		}else{
			ret->child[cIdx] = invertOctree(t->child[cIdx]);
		}
	}
	return ret;
}
