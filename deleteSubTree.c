#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "octreeOps.h"
#include "smalloc.h"

int setMeToNull = 0;//FIXME shouldn't exist. instead, take a oct** t instead of single pointer

void deleteSubTree(oct* t, int x, int y, int z, int mag){
	if(t == NULL){
		puts("Deleting failed. though this is supported by the spec, it shouldn't happen in the current version");
		return;
	}
	if(t->mag == mag){
		freeOctree(t);
		setMeToNull = 1;//Signals to parent function to set me to NULL
		return;
	}

	if(t->full){
		t->full = 0;
		for(int c = 0; c < 8; c++){//we create 8 full children
			t->child[c] = smalloc(sizeof(oct));
			t->child[c]->mag = t->mag-1;
			t->child[c]->full = 1;
			memset(t->child[c]->child, 0, 8*sizeof(oct*));
			getCubeCorner(c, t, t->child[c]->corner);
		}
	}
	int cIdx = identifyCorner(t, x, y, z);
	int offset = 1<<(t->mag-1);
	if(x >= offset) x-=offset;
	if(y >= offset) y-=offset;
	if(z >= offset) z-=offset;
	deleteSubTree(t->child[cIdx], x, y, z, mag);
	if(setMeToNull){
		setMeToNull = 0;
		t->child[cIdx] = NULL;
	}
}
