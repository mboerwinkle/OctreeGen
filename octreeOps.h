#include "structures.h"
#ifndef OCTREEOPS_H
#define OCTREEOPS_H
extern void freeOctree(oct* target);
extern oct* invertOctree(oct* t);
extern oct* duplicateOctree(oct* t);
extern oct* findContiguousOctree(oct* t, int x, int y, int z);
extern int cornerExists(oct* t, int x, int y, int z);
extern void addCorner(oct* t, int x, int y, int z);
extern void getCubeCorner(int idx, oct* parent, oct* target);
extern int identifyCorner(oct* t, int x, int y, int z);
extern void addTriangle(oct* t, facet* tri, double res);
#endif
