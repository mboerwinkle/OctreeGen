#ifndef STRUCTURES_H
#define STRUCTURES_H
#include <inttypes.h>
extern double resolution;
extern int magnitude;
extern char expandflag;
typedef struct facet{
	float vec[3];
	float p1[3];
	float p2[3];
	float p3[3];
	uint16_t attr;
}__attribute__((packed)) facet;

typedef struct oct{
	int mag;//side length of voxel is (RESOLUTION*2^mag)
	int full;//if it exists, it is either partial or full.
	int corner[3];
	struct oct* child[8];
}oct;
extern oct* getChild(oct* parent, int x, int y, int z);//x, y, and z, are 1 or -1 to specify which corner.

typedef struct model{
	facet* facets;
	uint32_t facetCount;
	float min[3];
	float max[3];
	oct* myTree;
}model;

#endif
