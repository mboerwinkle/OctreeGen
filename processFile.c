#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "octreeOps.h"

model loadFile(FILE* input);
void writeOutput(FILE* output, model target);
extern model generateOctree(model target);
extern void createCubeTriangles(double* center, double sideLen, facet* facetArray);
void writeCubeOutput(FILE* output, oct* tree, int dummy);

void processFile(FILE* inputFile, FILE* outputFile){
	model target = loadFile(inputFile);//create a model structure from the stl
	target = generateOctree(target);//populates the model's octree
	oct* inverse = invertOctree(target.myTree);
	freeOctree(target.myTree);
	target.myTree = NULL;
	oct* inverseContiguous = findContiguousOctree(inverse, 0, 0, 0);
	freeOctree(inverse);
	inverse = NULL;
	target.myTree = invertOctree(inverseContiguous);//leaving you with just the contiguous
	freeOctree(inverseContiguous);
	inverseContiguous = NULL;
	writeOutput(outputFile, target);//write the octree to file
	free(target.facets);
	freeOctree(target.myTree);
}
model loadFile(FILE* input){//This does not require the fancy stuff done during rendering where triangles have point references. We are doing this quick and dirty
	model ret;
	//skip header
	fseek(input, 80, SEEK_SET);
	//read triangle count
	fread(&(ret.facetCount), sizeof(uint32_t), 1, input);
	printf("%d facets\n", ret.facetCount);
	//allocate space for the triangles
	ret.facets = calloc(ret.facetCount, sizeof(facet));
	//read in all the triangles
	fread(ret.facets, sizeof(facet), ret.facetCount, input);
	fclose(input);
	return ret;
}
int triangleWriteCount = 0;
void writeOutput(FILE* output, model target){
	puts("Dummy Write");
	if(target.myTree != NULL) writeCubeOutput(output, target.myTree, 1);
	printf("Dummy Write found %d triangles. Press Enter to continue, or ctrl+c to abort\n", triangleWriteCount);
	getchar();
	triangleWriteCount = 0;
	puts("Writing output");
	char stlHeader[84] = {'F'};
	fwrite(stlHeader, 1, 84, output);//Include 4 bytes for the number of faces
	if(target.myTree != NULL) writeCubeOutput(output, target.myTree, 0);
	fseek(output, 80, SEEK_SET);//return to the number of faces
	fwrite(&triangleWriteCount, sizeof(int), 1, output);
	printf("triangles: %d\n", triangleWriteCount);
	fclose(output);
	puts("Output written");
}
void writeCubeOutput(FILE* output, oct* tree, int dummy){
	if(tree->full == 1){
		//if(tree->mag != 0)printf("we have a 'full' of mag %d\n", tree->mag);
		double center[3] = {resolution*tree->corner[0], resolution*tree->corner[1], resolution*tree->corner[2]};
		for(int dim = 0; dim < 3; dim++){
			center[dim]+= resolution*(1<<(tree->mag))*0.5;//FIXME code dupe
		}
		double sideLen = resolution*(1<<(tree->mag));
	//	printf("center %lf %lf %lf\n", center[0], center[1], center[2]);
		facet cube[12];
		//facet* cube = calloc(12, sizeof(facet));
		createCubeTriangles(center, sideLen, cube);
		if(!dummy) fwrite(cube, sizeof(facet), 12, output);
		triangleWriteCount+=12;
	}
	for(int cIdx = 0; cIdx < 8; cIdx++){
		if(tree->child[cIdx] != NULL){
			writeCubeOutput(output, tree->child[cIdx], dummy);
		}
	}
}
