#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
#include "octreeOps.h"

oct* getSubOctree(oct* t, int x, int y, int z, int mag){
	if(mag > t->mag){
		puts("Fatal error #451");
		return NULL;//this should never happen
	}
	if(mag == t->mag){
		if(x != t->corner[0] || y != t->corner[1] || z != t->corner[2]){
			puts("fatal error # 452");
			return NULL;//we somehow are at the correct level, but in the wrong place
		}
		return t;
	}
	int cIdx = identifyCorner(t, x, y, z);
	if(t->child[cIdx] == NULL) return NULL;//there is no neighbor on that side
	int cSideLen = 1<<(t->mag-1);
	if(x >= cSideLen) x-= cSideLen;
	if(y >= cSideLen) y-= cSideLen;
	if(z >= cSideLen) z-= cSideLen;
	return getSubOctree(t->child[cIdx], x, y, z, mag);

}
