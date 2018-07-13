#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "octreeOps.h"
//FIXME better implementation has a balanced binary tree of locations that is also an array. fRef keeps track of array index of frontier queue locations. using an octree for this application is inefficient since both fRef and the octree store location data for the frontier
//FIXME better implementation can take advantage of full sectors.
//FIXME make non-recursive versions of popular octree functions to see which is faster
typedef struct fRef{
	int l[3];
	int mag;
	int dir;//x-, x+, y-, y+, z-, z+. this way, when we have to downgrade our magnitude, we know which two to add
	struct fRef* next;
}fRef;

fRef *fEnqueue = NULL, *fDequeue = NULL;
void enqueue(int x, int y, int z, int mag, int dir){
	if(fEnqueue == NULL){
		fEnqueue = malloc(sizeof(fRef));
		fDequeue = fEnqueue;
	}else{
		fEnqueue->next = malloc(sizeof(fRef));//FIXME this shouldn't be duplicated. sloppy sloppy
		fEnqueue = fEnqueue->next;
	}
	fEnqueue->l[0] = x;
	fEnqueue->l[1] = y;
	fEnqueue->l[2] = z;
	fEnqueue->mag = mag;
	fEnqueue->dir = dir;
	fEnqueue->next = NULL;
}
void dequeue(int *x, int *y, int *z, int *mag, int *dir){
	*x = fDequeue->l[0];//FIXME operate on a fRef pointer instead of each element individually
	*y = fDequeue->l[1];
	*z = fDequeue->l[2];
	*mag = fDequeue->mag;
	*dir = fDequeue->dir;
	if(fDequeue == fEnqueue){
		fEnqueue = NULL;
	}
	fRef* old = fDequeue;
	fDequeue = fDequeue->next;
	free(old);
}

oct* findContiguousOctree(oct* t, int x, int y, int z){
	puts("findContiguousOctree called");
	//check if that corner exists
	if(!cornerExists(t, x, y, z)){
		puts("findContiguousOctree called. Result is null tree");
		return NULL;
	}
	oct* ret = malloc(sizeof(oct));//FIXME make this blank initialization a function instead of duplicating it a ton.
	ret->full = 0;
	ret->mag = t->mag;
	memset(ret->child, 0, 8*sizeof(oct*));
	memcpy(ret->corner, t->corner, 3*sizeof(int));
	//if it exists, add it to the frontier
	oct* f = malloc(sizeof(oct));//We store the frontier as an octree, since it is easily searchable, and easily editable. In addition, we keep a queue of locations for proper frontier action. Keep in mind that the frontier octree also includes all locations that have ever been part of the frontier. By the end, it will be identical to the returned octree, plus an outer shell.
	f->full = 0;
	f->mag = t->mag;
	memset(f->child, 0, 8*sizeof(oct*));
	addCorner(f, x, y, z, 0);
	enqueue(x, y, z, 0, 0);//FIXME would it be better to start with a high magnitude?
	long int totalSize = 0;
	//frontier expansion loop
	while(fDequeue != NULL){//while there are still unexplored frontiers
		int fx, fy, fz, mag, dir;
		dequeue(&fx, &fy, &fz, &mag, &dir);
		if(cornerExists(t, fx, fy, fz)){
			addCorner(ret, fx, fy, fz, 0);
			totalSize++;//FIXME there is an error in the total size calculation. it is too big. check if addCorner is adding spots that have already been added?
			if(!cornerExists(f, fx-1, fy, fz)){//invalid is -1. this counts as true. so we basically treat invalid spots as existing and everyone is happy
				addCorner(f, fx-1, fy, fz, 0);
				enqueue(fx-1, fy, fz, 0, 0);
			}
			if(!cornerExists(f, fx+1, fy, fz)){
				addCorner(f, fx+1, fy, fz, 0);
				enqueue(fx+1, fy, fz, 0, 1);
			}
			if(!cornerExists(f, fx, fy-1, fz)){
				addCorner(f, fx, fy-1, fz, 0);
				enqueue(fx, fy-1, fz, 0, 2);
			}
			if(!cornerExists(f, fx, fy+1, fz)){
				addCorner(f, fx, fy+1, fz, 0);
				enqueue(fx, fy+1, fz, 0, 3);
			}
			if(!cornerExists(f, fx, fy, fz-1)){
				addCorner(f, fx, fy, fz-1, 0);
				enqueue(fx, fy, fz-1, 0, 4);
			}
			if(!cornerExists(f, fx, fy, fz+1)){
				addCorner(f, fx, fy, fz+1, 0);
				enqueue(fx, fy, fz+1, 0, 5);
			}
		}
	}
	freeOctree(f);
	printf("contiguous found a volume of %ld\n", totalSize);
	return ret;
}
