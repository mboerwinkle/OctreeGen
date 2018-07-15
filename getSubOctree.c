#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
#include "octreeOps.h"
//FIXME there is a large problem where sometimes the low corner of a cube is referred to as 0, and sometimes -sideLen. We should really be using 0. Fix everything please.

oct* getSubOctree(oct* t, int x, int y, int z, int mag, int* full){
//	printf("called getSubOctree %p, %d %d %d, mag: %d\n", t, x, y, z, mag);
	if(mag > t->mag){
		puts("Fatal error #451");
		return NULL;//this should never happen
	}
	if(mag == t->mag){
//FIXME this cannot exist until I fix the fixme above		if(x != t->corner[0] || y != t->corner[1] || z != t->corner[2]){
//			puts("fatal error # 452");
//			return NULL;//we somehow are at the correct level, but in the wrong place
//		}
		if(full != NULL) *full = t->full;
		return t;
	}
	int cIdx = identifyCorner(t, x, y, z);
	if(t->child[cIdx] == NULL){
		if(full != NULL) *full = t->full;
		return NULL;//there is no neighbor on that side
	}
	int cSideLen = 1<<(t->mag-1);
	if(x >= cSideLen) x-= cSideLen;
	if(y >= cSideLen) y-= cSideLen;
	if(z >= cSideLen) z-= cSideLen;
	return getSubOctree(t->child[cIdx], x, y, z, mag, full);

}
