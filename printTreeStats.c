#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
#include "octreeOps.h"

int* cubeCount = NULL;
void printTreeStats(oct* t){
	int topLevel = 0;
	if(cubeCount == NULL){
		cubeCount = calloc(t->mag+1, sizeof(int));
		topLevel = 1;
	}
	if(t->full){
		(cubeCount[t->mag])++;
	}else{
		for(int c = 0; c < 8; c++){
			if(t->child[c] != NULL){
				printTreeStats(t->child[c]);
			}
		}
	}
	if(topLevel){
		puts("CUBE BREAKDOWN");
		for(int x = t->mag; x >= 0; x--){
			printf("%d: %d\n", x, cubeCount[x]);
		}
		free(cubeCount);
	}
}
