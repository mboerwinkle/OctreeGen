#include "structures.h"
#ifndef OCTREEOPS_H
#define OCTREEOPS_H
//frees this oct and all of its children
extern void freeOctree(oct* target);
//creates an octree that is the exact opposite (like a mold)
extern oct* invertOctree(oct* t);
//creates a duplicate of the target
extern oct* duplicateOctree(oct* t);
//creates an octree that is a flood-fill of the selected point
extern oct* findContiguousOctree(oct* t, int x, int y, int z);
//returns 1 if that corner exists, or 0 if it doesn't
extern int cornerExists(oct* t, int x, int y, int z);
//adds a lowest-level voxel to the octree at the specified location
extern void addCorner(oct* t, int x, int y, int z);
//writes correct cube corner data to target, assuming parent is its parent and it is child[idx]
extern void getCubeCorner(int idx, oct* parent, oct* target);
//given a point inside of a cube, identifies which child it lies in
extern int identifyCorner(oct* t, int x, int y, int z);
//adds all of the lowest level voxels that intersect the given facet to the octree
extern void addTriangle(oct* t, facet* tri, double res);
//determines which of the faces of the specified cube are open to the air. returns the number of exposed faces
extern int exposedFaces(oct* tree, int* corner, int mag, int* faces);

#endif
