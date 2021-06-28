#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "structures.h"
#include "octreeOps.h"

unsigned long int* cubeCount;
unsigned long int cubeVolume;

void printTreeStatsRec(subtree* t){
	char val = getStatus(t);
	assert(val == 'F' || val == 'P' || val == 'E');
	if(val == 'E'){
		return;
	}else if(val == 'F'){
		(cubeCount[t->mag])++;
		unsigned long int sideLen = sidelen(t->mag);
		cubeVolume += sideLen * sideLen * sideLen;
	}else{//partial
		assert(!(val == 'P' && t->mag == 0));
		for(int c = 0; c < 8; c++){
			subtree child = childSubtree(t, c);
			printTreeStatsRec(&child);
		}
	}
}
char prefixes[6] = {' ','K','M','G','T','P'};
void printTreeStats(oct* t){
	cubeCount = calloc(t->mag+1, sizeof(unsigned long int));
	cubeVolume = 0;
	subtree root = rootSubtree(t);
	printTreeStatsRec(&root);
	long int rootsidelen = sidelen(root.mag);
	unsigned long int totalVolume = rootsidelen*rootsidelen*rootsidelen;
	fprintf(stderr, "CUBE BREAKDOWN\n");
	for(int x = t->mag; x >= 0; x--){
		fprintf(stderr, "%d: %ld\n", x, cubeCount[x]);
	}
	double size = t->tusage;
	int prefixidx = 0;
	while(size >= 1024.0){
		size /= 1024.0;
		prefixidx++;
	}
	fprintf(stderr,
		"CORNER AT: (%ld, %ld, %ld), "
		"CUBE VOLUME: %ld (%lf%%), "
		"%lf %cBYTES\n",
		root.corner.l[0], root.corner.l[1], root.corner.l[2],
		cubeVolume, 100.0*(double)cubeVolume/(double)totalVolume,
		size, prefixes[prefixidx]
	);
	free(cubeCount);
}
