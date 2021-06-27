#include "structures.h"
#ifndef OCTREEOPS_H
#define OCTREEOPS_H
//creates an octree that is a flood-fill of the selected point
extern oct* findContiguousOctree(oct* t, pt corner);
//sets all empty cubes which border a partial cube to partial. Sets all full cubes that border a partial or empty to partial. result is not a strictly valid octree.
extern oct* expandOctree(oct* t);

// #cornerExists.c
//returns 1 if that corner exists, or 0 if it doesn't
extern char cornerExists(oct* t, pt corner, int mag, int* foundMagnitude, pt* foundCorner);
extern char cornerExistsRec(subtree* t, pt corner, int mag, int* foundMagnitude, pt* foundCorner);
//adds a lowest-level voxel to the octree at the specified location
extern void addCorner(oct* t, pt corner, int mag);
extern void addCornerRec(subtree* t, pt corner, int mag);
//given a point inside of a cube, identifies which child it lies in
extern int identifyCorner(subtree* parent, pt internal);

//adds all of the lowest level voxels that intersect the given facet to the octree
extern int addTriangle(subtree* t, facet* tri, double res);
//prints out statistics about the tree
extern void printTreeStats(oct* t);
extern void writeStlOutput(FILE* output, oct* tree);
#endif
