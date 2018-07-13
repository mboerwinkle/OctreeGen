#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "octreeOps.h"

void addCorner(oct* t, int x, int y, int z, int mag){
	if(t->mag == mag){
		t->full = 1;
		for(int cIdx = 0; cIdx < 8; cIdx++){//FIXME function to free all children
			if(t->child[cIdx] != NULL) freeOctree(t->child[cIdx]);
		}
		memset(t->child, 0, 8*sizeof(oct*));
		return;
	}
	int cIdx = identifyCorner(t, x, y, z);
	//create target child if it doesn't exist
	if(t->child[cIdx] == NULL){
		t->child[cIdx] = malloc(sizeof(oct));
		t->child[cIdx]->mag = t->mag-1;
		t->child[cIdx]->full = 0;
		memset(t->child[cIdx]->child, 0, 8*sizeof(oct*));
		getCubeCorner(cIdx, t, t->child[cIdx]);
	}
	//recurse
	int sideLen = 1<<(t->mag-1);//FIXME code duplication
	if(x >= sideLen) x-=sideLen;
	if(y >= sideLen) y-=sideLen;
	if(z >= sideLen) z-=sideLen;
	addCorner(t->child[cIdx], x, y, z, mag);
	//test if you are full
	int fullChildCount = 0;
	for(int testCIdx = 0; testCIdx < 8; testCIdx++){
		if(t->child[testCIdx] != NULL && t->child[testCIdx]->full){
			fullChildCount++;
		}else{
			break;
		}
	}
	if(fullChildCount == 8){
		t->full = 1;
		for(int killCIdx = 0; killCIdx < 8; killCIdx++){
			freeOctree(t->child[killCIdx]);
		}
		memset(t->child, 0, 8*sizeof(oct*));
	}
}
