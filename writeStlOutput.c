#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "structures.h"
#include "octreeOps.h"

//this flag culls cube faces that are completely obscured by other faces. Much smaller stl on highly contiguous models, at the expense of computation time
#define OCTREE_WRITE_CULL
#define TRIANGLEMAXWRITE 4000000

extern void createCubeTriangles(double* center, double sideLen, facet* facetArray);

int faceExposedRec(subtree* t, char dir){
	assert(getStatus(t) == 'P' && t->mag >= 1);
	long int o = sidelen(t->mag-1);//offset
	int primaryaxis = dir/2;// 0,1->0; 2,3->1; 4,5->2
	int otheraxes[DIM-1];
	for(int idx = 1; idx < DIM; idx++){
		otheraxes[idx-1] = (primaryaxis+idx)%DIM;
	}
	int primaryside = -(dir%2-1);//evens->1, odds->0
	for(int idx = 0; idx < (1<<(DIM-1)); idx++){
		pt alteredpt = t->corner;
		alteredpt.l[primaryaxis] += primaryside*o;
		for(int aidx = 0; aidx < DIM-1; aidx++){
			alteredpt.l[otheraxes[aidx]] += o*((idx/(aidx+1))%2);
		}
		int cIdx = identifyCorner(t, alteredpt);
		subtree child = childSubtree(t, cIdx);
		char cstatus = getStatus(&child);
		if(cstatus == 'E'){
			return 1;
		}
		if(cstatus == 'P' && faceExposedRec(&child, dir)){
			return 1;
		}
	}
	return 0;
}

int exposedFaces(subtree* tree, char* faces){
	memset(faces, 0, sizeof(char)*6);
	long int sideLen = sidelen(tree->mag);
	int total = 0;
	for(int neighborIdx = 0; neighborIdx < 6; neighborIdx++){
		pt testpt = tree->corner;
		int axis = neighborIdx / 2;
		long int dir = neighborIdx%2*2-1;
		testpt.l[axis] += dir*sideLen;
		int neighborExists = cornerExists(tree->origin, testpt, tree->mag, NULL, NULL);
		//if the neighbor is empty, it exists.
		if(neighborExists < 1){
			faces[neighborIdx] = 1;
			total++;
		//if the neighbor is partial, do an expensive recursion
		}else if(neighborExists == 1){
			subtree neighbor = cornerSubtree(tree->origin, testpt, tree->mag);
			if(faceExposedRec(&neighbor, neighborIdx)){
				faces[neighborIdx] = 1;
				total++;
			}
		}
	}
	return total;
}
long int triangleWriteCount = 0;
int writeTreeTrianglesRec(FILE* output, subtree* t){
	assert(t->mag >= 0);
	char status = getStatus(t);
	if(status == 'F'){
		double sideLen = resolution * (double)sidelen(t->mag);
		//This cannot be computed from sidelen(t->mag-1) because it may be 0.5
		double childsidelen = sideLen/2.0;
		double center[3] = {
			childsidelen+(resolution*t->corner.l[0]),
			childsidelen+(resolution*t->corner.l[1]),
			childsidelen+(resolution*t->corner.l[2])
		};
		facet cube[12];
		createCubeTriangles(center, sideLen, cube);
		char faces[6] = {1, 1, 1, 1, 1, 1};//x-, x+, y-, y+, z-, z+
		#ifdef OCTREE_WRITE_CULL
		triangleWriteCount+=2*exposedFaces(t, faces);
		#else
		triangleWriteCount+=12;
		#endif
		for(int fIdx = 0; fIdx < 6; fIdx++){
			if(faces[fIdx]) fwrite(&(cube[fIdx*2]), sizeof(facet), 2, output);
		}
		if(triangleWriteCount > TRIANGLEMAXWRITE){
			fprintf(stderr, "too many triangles (>%d). Aborting.\n", TRIANGLEMAXWRITE);
			fprintf(stderr, "wrote %ld triangles\n", triangleWriteCount);
			return 254;
		}
	}else if(status == 'P'){
		for(int cIdx = 0; cIdx < 8; cIdx++){
			subtree child = childSubtree(t, cIdx);
			int ret = writeTreeTrianglesRec(output, &child);
			if(ret == 254){
				return 254;
			}
		}
	}
	return 0;
}
void writeStlOutput(FILE* output, oct* tree){
	triangleWriteCount = 0;
	fprintf(stderr, "Writing output\n");
	char stlHeader[84] = {'F'};
	fwrite(stlHeader, 1, 84, output);//Include 4 bytes for the number of faces
	subtree root = rootSubtree(tree);
	writeTreeTrianglesRec(output, &root);
	fseek(output, 80, SEEK_SET);//return to the number of faces
	fwrite(&triangleWriteCount, sizeof(int), 1, output);
	fflush(output);
	fprintf(stderr, "triangles: %ld\n", triangleWriteCount);
	fprintf(stderr, "Output written\n");
}
