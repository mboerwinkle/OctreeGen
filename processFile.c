#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "structures.h"
#include "octreeOps.h"

model loadFile(FILE* input);
extern oct* generateOctree(model target, double modelSize);
void writeRawOctree(FILE* output, oct* tree);

void processFile(FILE* inputFile, FILE* outputFile, double modelSize){
	model target = loadFile(inputFile);//create a model structure from the stl
	oct* tree = generateOctree(target, modelSize);
	#ifndef NDEBUG
		validateOctree(tree);
	#endif
	free(target.facets);
	printTreeStats(tree);
	oct* inverse = invertOctree(tree);
	#ifndef NDEBUG
		validateOctree(inverse);
	#endif
	freeOctree(tree);
	tree = NULL;
	printTreeStats(inverse);
	long int sideLen = sidelen(inverse->mag);
	oct* inverseContiguous = findContiguousOctree(inverse, (pt){.l={-sideLen/2, -sideLen/2, -sideLen/2}});
	#ifndef NDEBUG
		validateOctree(inverseContiguous);
	#endif
	freeOctree(inverse);
	inverse = NULL;
	printTreeStats(inverseContiguous);
	tree = invertOctree(inverseContiguous);//leaving you with just the contiguous
	#ifndef NDEBUG
		validateOctree(tree);
	#endif
	freeOctree(inverseContiguous);
	inverseContiguous = NULL;
	printTreeStats(tree);
	if(expandflag){
		//expanded is guaranteed to fail validateOctree
		oct* expanded = expandOctree(tree);
		freeOctree(tree);
		tree = expanded;
		expanded = NULL;
		printTreeStats(tree);
	}
	if(stlflag){
		//write the octree to file as stl
		writeStlOutput(outputFile, tree);
	}else{
		writeRawOctree(outputFile, tree);
	}
	fclose(outputFile);
	freeOctree(tree);
}
void writeRawOctree(FILE* output, oct* tree){
	fprintf(output, "NHC3_%04X_", tree->mag);
	for(long unsigned int offset = 0; offset < tree->tusage; offset++){
		fputc(tree->t[offset], output);
	}
	fflush(output);
}
model loadFile(FILE* input){//This does not require the fancy stuff done during rendering where triangles have point references. We are doing this quick and dirty
	model ret;
	//skip header
	fseek(input, 80, SEEK_SET);
	//read triangle count
	fread(&(ret.facetCount), sizeof(uint32_t), 1, input);
	fprintf(stderr, "%d facets\n", ret.facetCount);
	//allocate space for the triangles
	ret.facets = calloc(ret.facetCount, sizeof(facet));
	//read in all the triangles
	fread(ret.facets, sizeof(facet), ret.facetCount, input);
	fclose(input);
	return ret;
}
