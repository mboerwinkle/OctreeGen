#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
#include "octreeOps.h"

void printOctree(oct* t){
	for(int x = 0; x < t->mag; x++) printf("  ");
	printf("%d %d %d\n", t->corner[0], t->corner[1], t->corner[2]);
	if(t->full) return;
	for(int cIdx = 0; cIdx < 8; cIdx++){
		if(t->child[cIdx] == NULL){
			for(int x = 0; x < t->mag-1; x++) printf("  ");
			printf("NULL\n");
		}else{
			printOctree(t->child[cIdx]);
		}
	}
}
