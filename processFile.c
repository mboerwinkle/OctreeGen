#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "octreeOps.h"

//this flag culls cube faces that are completely obscured by other faces. Much smaller stl on highly contiguous models, at the expense of computation time
#define OCTREE_WRITE_CULL
model loadFile(FILE* input);
void writeOutput(FILE* output, model target);
extern model generateOctree(model target, double modelSize);
extern void createCubeTriangles(double* center, double sideLen, facet* facetArray);
void writeCubeOutput(FILE* output, oct* tree);
void writeRawOctree(FILE* output, oct* tree);

void processFile(FILE* inputFile, FILE* outputFile, double modelSize){
	model target = loadFile(inputFile);//create a model structure from the stl
	target = generateOctree(target, modelSize);//populates the model's octree
	oct* inverse = invertOctree(target.myTree);
	freeOctree(target.myTree);
	target.myTree = NULL;
	oct* inverseContiguous = findContiguousOctree(inverse, 0, 0, 0);
	freeOctree(inverse);
	inverse = NULL;
	target.myTree = invertOctree(inverseContiguous);//leaving you with just the contiguous
	freeOctree(inverseContiguous);
	inverseContiguous = NULL;
	printTreeStats(target.myTree);
	//writeOutput(outputFile, target);//write the octree to file as stl
	fputc('3', outputFile);
	writeRawOctree(outputFile, target.myTree);//write the octree to file as ocls
	fflush(outputFile);
	free(target.facets);
	freeOctree(target.myTree);
	fclose(outputFile);
}

void writeRawOctree(FILE* output, oct* tree){
	if(tree->full){
		fputc('F', output);
	}else{
		fputc('P', output);
		for(int c = 0; c < 8; c++){
			if(tree->child[c] == NULL){
				fputc('E', output);
			}else{
				writeRawOctree(output, tree->child[c]);
			}
		}
	}
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
//	printOctree(target.myTree);
	triangleWriteCount = 0;
	puts("Writing output");
	char stlHeader[84] = {'F'};
	fwrite(stlHeader, 1, 84, output);//Include 4 bytes for the number of faces
	if(target.myTree != NULL) writeCubeOutput(output, target.myTree);
	fseek(output, 80, SEEK_SET);//return to the number of faces
	fwrite(&triangleWriteCount, sizeof(int), 1, output);
	printf("triangles: %d\n", triangleWriteCount);
	puts("Output written");
}
oct* writeCubeOutputMasterTree = NULL;
void writeCubeOutput(FILE* output, oct* tree){
	if(writeCubeOutputMasterTree == NULL) writeCubeOutputMasterTree = tree;
	if(tree->full == 1){
		double center[3] = {resolution*tree->corner[0], resolution*tree->corner[1], resolution*tree->corner[2]};
		for(int dim = 0; dim < 3; dim++){
			center[dim]+= resolution*(1<<(tree->mag))*0.5;//FIXME code dupe
		}
		double sideLen = resolution*(1<<(tree->mag));
		facet cube[12];
		createCubeTriangles(center, sideLen, cube);
		int faces[6] = {1, 1, 1, 1, 1, 1};//x-, x+, y-, y+, z-, z+
		#ifdef OCTREE_WRITE_CULL
		triangleWriteCount+=2*exposedFaces(writeCubeOutputMasterTree, tree->corner, tree->mag, faces);
		#else
		triangleWriteCount+=12;
		#endif
		for(int fIdx = 0; fIdx < 6; fIdx++){
			if(faces[fIdx]) fwrite(&(cube[fIdx*2]), sizeof(facet), 2, output);
		}
		if(triangleWriteCount > 4000000){
			puts("too many triangles (>4000000). Aborting.");
			printf("wrote %d triangles\n", triangleWriteCount);
			fclose(output);
			exit(1);
		}
	}
	for(int cIdx = 0; cIdx < 8; cIdx++){
		if(tree->child[cIdx] != NULL){
			writeCubeOutput(output, tree->child[cIdx]);
		}
	}
	if(writeCubeOutputMasterTree == tree) writeCubeOutputMasterTree = NULL;
}
int exposedFaces(oct* tree, int* corner, int mag, int* faces){
//	printf("exposedFaces %d %d %d, mag %d\n", corner[0], corner[1], corner[2], mag);
	memset(faces, 0, sizeof(int)*6);
	int sideLen = 1<<mag;
	int sL = 1<<(tree->mag-1);//this is used for hacky offset. see fixme below
	oct *xp, *xn, *yp, *yn, *zp, *zn;
	//full says if that face is blocked by a full cube of a higher magnitude than itself
	int full[6];
	//FIXME all of the sL's are a product of the FIXMe referenced int getSubOctree.c
	xn = getSubOctree(tree, corner[0]+sL-sideLen, corner[1]+sL, corner[2]+sL, mag, &(full[0]));
	xp = getSubOctree(tree, corner[0]+sL+sideLen, corner[1]+sL, corner[2]+sL, mag, &(full[1]));
	yn = getSubOctree(tree, corner[0]+sL, corner[1]+sL-sideLen, corner[2]+sL, mag, &(full[2]));
	yp = getSubOctree(tree, corner[0]+sL, corner[1]+sL+sideLen, corner[2]+sL, mag, &(full[3]));
	zn = getSubOctree(tree, corner[0]+sL, corner[1]+sL, corner[2]+sL-sideLen, mag, &(full[4]));
	zp = getSubOctree(tree, corner[0]+sL, corner[1]+sL, corner[2]+sL+sideLen, mag, &(full[5]));
//	printf("%p %p %p %p %p %p\n", xp, xn, yp, yn, zp, zn);
//	printf("%d %d %d %d %d %d\n", full[0], full[1], full[2], full[3], full[4], full[5]);
	//this chunk of code is iterating over the face of the cube, and determining if each point on the surface exists. the "!faces[x] &&" is basically an early exit to make it less intensive once it is determined that the face is needed.
	for(int d1 = 0; d1 < sideLen; d1++){
		for(int d2 = 0; d2 < sideLen; d2++){
			if(!full[0] && !faces[0] && !cornerExists(xn, sideLen-1, d1, d2, 0, NULL)){
				faces[0] = 1;
			}
			if(!full[1] && !faces[1] && !cornerExists(xp, 0, d1, d2, 0, NULL)){
				faces[1] = 1;
			}
			if(!full[2] && !faces[2] && !cornerExists(yn, d1, sideLen-1, d2, 0, NULL)){
				faces[2] = 1;
			}
			if(!full[3] && !faces[3] && !cornerExists(yp, d1, 0, d2, 0, NULL)){
				faces[3] = 1;
			}
			if(!full[4] && !faces[4] && !cornerExists(zn, d1, d2, sideLen-1, 0, NULL)){
				faces[4] = 1;
			}
			if(!full[5] && !faces[5] && !cornerExists(zp, d1, d2, 0, 0, NULL)){
				faces[5] = 1;
			}
		}
	}
	int total = 0;
	for(int fIdx = 0; fIdx < 6; fIdx++){
		if(faces[fIdx]) total++;
	}
	return total;
}
