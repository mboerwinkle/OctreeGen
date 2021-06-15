#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
#include "octreeOps.h"

int* cubeCount = NULL;
unsigned long int cubeVolume = 0;

void printTreeStatsRec(oct* t){
	if(t->full){
		(cubeCount[t->mag])++;
		int sidelen = 1<<t->mag;
		cubeVolume += sidelen * sidelen * sidelen;
	}else{
		for(int c = 0; c < 8; c++){
			if(t->child[c] != NULL){
				printTreeStatsRec(t->child[c]);
			}
		}
	}
}
void printTreeStats(oct* t){
	cubeCount = calloc(t->mag+1, sizeof(int));
	printTreeStatsRec(t);
	fprintf(stderr, "CUBE VOLUME: %ld\n", cubeVolume);
	fprintf(stderr, "CUBE BREAKDOWN\n");
	for(int x = t->mag; x >= 0; x--){
		fprintf(stderr, "%d: %d\n", x, cubeCount[x]);
	}
	free(cubeCount);
	cubeCount = NULL;
}
