#include <stdio.h>
#include <stdlib.h>
#include "structures.h"

void freeOctree(oct* target){
	for(int cIdx = 0; cIdx < 8; cIdx++){
		if(target->child[cIdx] != NULL){
			freeOctree(target->child[cIdx]);
		}
	}
	free(target);
}
