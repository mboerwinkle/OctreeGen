#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "structures.h"

double normalize(float* dir);
extern int intersect_triangle(float orig[3], float dir[3], float vert0[3], float vert1[3], float vert2[3], double *t, double *u, double *v);
extern void createCubeTriangles(double* center, double sideLen, facet* facetArray);
int lineTriangleIntersect(float* l1, float* l2, float* t1, float* t2, float* t3);
int triangleTriangleIntersect(float *a1, float *a2, float *a3, float *b1, float *b2, float *b3);
int pointCubeIntersect(float* p, double* center, double sideLen);

int cubeTriangleIntersect(double* center, double sideLen, facet triangle){
	float tp1[3];
	float tp2[3];
	float tp3[3];
	for(int idx = 0; idx < 3; idx++){//This is BS to avoid issues with packed structure member pointers - this should be redone by having an unpacking phase early on instead FIXME
		tp1[idx] = triangle.p1[idx];
		tp2[idx] = triangle.p2[idx];
		tp3[idx] = triangle.p3[idx];
	}
	if(pointCubeIntersect(tp1, center, sideLen)||pointCubeIntersect(tp2, center, sideLen)||pointCubeIntersect(tp3, center, sideLen)){
		return 1;
	}
	facet cubeFacets[12];
	createCubeTriangles(center, sideLen, cubeFacets);
	for(int f = 0; f < 12; f++){
		float cp1[3];
		float cp2[3];
		float cp3[3];
		for(int idx = 0; idx < 3; idx++){//This is BS to avoid issues with packed structure member pointers - this should be redone by having an unpacking phase early on instead FIXME
			cp1[idx] = cubeFacets[f].p1[idx];
			cp2[idx] = cubeFacets[f].p2[idx];
			cp3[idx] = cubeFacets[f].p3[idx];
		}

		if(triangleTriangleIntersect(cp1,cp2,cp3,tp1,tp2,tp3)) return 1;
	}
	return 0;
}
int pointCubeIntersect(float* p, double* center, double sideLen){
	for(int dim = 0; dim < 3; dim++){
		if(fabs(p[dim]-center[dim]) > sideLen/2){
			return 0;
		}
	}
	return 1;
}
int triangleTriangleIntersect(float *a1, float *a2, float *a3, float *b1, float *b2, float *b3){
	if(lineTriangleIntersect(a1, a2, b1, b2, b3)||//FIXME use a dedicated 2 triangle intersector.
	lineTriangleIntersect(a2, a3, b1, b2, b3)||
	lineTriangleIntersect(a3, a1, b1, b2, b3)||
	lineTriangleIntersect(b1, b2, a1, a2, a3)||
	lineTriangleIntersect(b2, b3, a1, a2, a3)||
	lineTriangleIntersect(b3, b1, a1, a2, a3)){
		return 1;
	}
	return 0;
}

int lineTriangleIntersect(float* l1, float* l2, float* t1, float* t2, float* t3){
	float *orig, dir[3];
	double t, u, v;
	orig = l1;
	for(int dim = 0; dim < 3; dim++){
		dir[dim] = l2[dim]-l1[dim];
	}
	double len = normalize(dir);//FIXME is this necessary? or does it only scale returned t?
	int intersects = intersect_triangle(orig, dir, t1, t2, t3, &t, &u, &v);
	if(intersects && t < len && t >= 0){
		return 1;
	}
	return 0;
}

double normalize(float* vec){
	double len = sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
	vec[0]/=len;
	vec[1]/=len;
	vec[2]/=len;
	return len;
}
