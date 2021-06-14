#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"

oct* duplicateOctree(oct* t){
	if(t == NULL) return NULL;
	oct* ret = malloc(sizeof(oct));
	*ret = *t;
	if(ret->full) return ret;//FIXME I could have more early exits/optimizations
	for(int cIdx = 0; cIdx < 8; cIdx++){
		ret->child[cIdx] = duplicateOctree(t->child[cIdx]);
	}
	return ret;
}
