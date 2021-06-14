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
//sets all empty cubes which border a partial cube to partial. Sets all full cubes that border a partial or empty to partial. result is not a strictly valid octree.
extern oct* expandOctree(oct* t);
//returns 1 if that corner exists, or 0 if it doesn't
extern int cornerExists(oct* t, int x, int y, int z, int mag, int* foundMagnitude);
//adds a lowest-level voxel to the octree at the specified location
extern void addCorner(oct* t, int x, int y, int z, int mag);
//writes correct cube corner data to target, assuming parent is its parent and it is child[idx]
extern void getCubeCorner(int idx, oct* parent, oct* target);
//given a point inside of a cube, identifies which child it lies in
extern int identifyCorner(oct* t, int x, int y, int z);
//adds all of the lowest level voxels that intersect the given facet to the octree
extern void addTriangle(oct* t, facet* tri, double res);
//determines which of the faces of the specified cube are open to the air. returns the number of exposed faces
extern int exposedFaces(oct* tree, int* corner, int mag, int* faces);
//returns a pointer to the subtree of the given magnitude at the given position. If it does not exist, or it is full at a greater magnitude, then returns NULL. If full at a greater magnitude, full is set to 1, otherwise full is set to 0.
extern oct* getSubOctree(oct* t, int x, int y, int z, int mag, int* full);
//prints out the contents of the specified octree
extern void printOctree(oct* t);
//deletes the subtree of given magnitude from the specified octree (The opposite of addCorner)
extern void deleteSubTree(oct* t, int x, int y, int z, int mag);
//prints out statistics about the tree
extern void printTreeStats(oct* t);
extern void unionOctree(oct* target, oct* modifier);
#endif
