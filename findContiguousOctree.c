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
int queueLen = 0;
fRef *fEnqueue = NULL, *fDequeue = NULL;
void enqueue(int x, int y, int z, int mag, int dir){
	queueLen++;
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
	queueLen--;
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
	if(!cornerExists(t, x, y, z, 0, NULL)){
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
		printf("dequeued %d: %d %d %d mag: %d dir: %d\n", queueLen, fx, fy, fz, mag, dir);
		int foundMag;
		int cornerExistsRet = cornerExists(t, fx, fy, fz, mag, &foundMag);
		if(cornerExistsRet == 2){//exists fully
			printf("full at lvl %d\n", foundMag);
			int sideLen = 1<<foundMag;
			if(foundMag > mag){
				addCorner(f, fx, fy, fz, foundMag);
			}
			addCorner(ret, fx, fy, fz, foundMag);
			//totalSize++;//FIXME there is an error in the total size calculation. it is too big. check if addCorner is adding spots that have already been added?
			int fCornerExists = cornerExists(f, fx-sideLen, fy, fz, foundMag, NULL);
			if(fCornerExists == 0/* || fCornerExists == 1*/){
				addCorner(f, fx-sideLen, fy, fz, foundMag);
				enqueue(fx-sideLen, fy, fz, foundMag, 0);
			}
			fCornerExists = cornerExists(f, fx+sideLen, fy, fz, foundMag, NULL);
			if(fCornerExists == 0/* || fCornerExists == 1*/){
				addCorner(f, fx+sideLen, fy, fz, foundMag);
				enqueue(fx+sideLen, fy, fz, foundMag, 1);
			}
			fCornerExists = cornerExists(f, fx, fy-sideLen, fz, foundMag, NULL);
			if(fCornerExists == 0/* || fCornerExists == 1*/){
				addCorner(f, fx, fy-sideLen, fz, foundMag);
				enqueue(fx, fy-sideLen, fz, foundMag, 2);
			}
			fCornerExists = cornerExists(f, fx, fy+sideLen, fz, foundMag, NULL);
			if(fCornerExists == 0/* || fCornerExists == 1*/){
				addCorner(f, fx, fy+sideLen, fz, foundMag);
				enqueue(fx, fy+sideLen, fz, foundMag, 3);
			}
			fCornerExists = cornerExists(f, fx, fy, fz-sideLen, foundMag, NULL);
			if(fCornerExists == 0/* || fCornerExists == 1*/){
				addCorner(f, fx, fy, fz-sideLen, foundMag);
				enqueue(fx, fy, fz-sideLen, foundMag, 4);
			}
			fCornerExists = cornerExists(f, fx, fy, fz+sideLen, foundMag, NULL);
			if(fCornerExists == 0/* || fCornerExists == 1*/){
				addCorner(f, fx, fy, fz+sideLen, foundMag);
				enqueue(fx, fy, fz+sideLen, foundMag, 5);
			}
		}else if(cornerExistsRet == 1){//FIXME this should delete the subtree, then duplicate the ret tree to the stump. And then add the 4 new frontier full subtrees
			puts("partial, subdividing");
			int isFullTest;
			oct* target = getSubOctree(f, fx, fy, fz, mag, &isFullTest);
			if(target == NULL){
				printf("Im about to fail. Null cube full: %d\n", isFullTest);
				continue;
			}
			if(!target->full) puts("this shouldnt happen...");
			target->full = 0;
			int o = 1<<(mag-1);//offset
			if(dir == 0){//100, 101, 110, 111//FIXME remove code duplication
				//4, 5, 6, 7
				addCorner(target, o, 0, 0, mag-1);
				enqueue(fx+o, fy, fz, mag-1, dir);
				addCorner(target, o, 0, o, mag-1);
				enqueue(fx+o, fy, fz+o, mag-1, dir);
				addCorner(target, o, o, 0, mag-1);
				enqueue(fx+o, fy+o, fz, mag-1, dir);
				addCorner(target, o, o, o, mag-1);
				enqueue(fx+o, fy+o, fz+o, mag-1, dir);
			}
			if(dir == 1){//000, 001, 010, 011
				//0, 1, 2, 3
				addCorner(target, 0, 0, 0, mag-1);
				enqueue(fx, fy, fz, mag-1, dir);
				addCorner(target, 0, 0, o, mag-1);
				enqueue(fx, fy, fz+o, mag-1, dir);
				addCorner(target, 0, o, 0, mag-1);
				enqueue(fx, fy+o, fz, mag-1, dir);
				addCorner(target, 0, o, o, mag-1);
				enqueue(fx, fy+o, fz+o, mag-1, dir);
			}
			if(dir == 2){//010, 011, 110, 111
				//2, 3, 6, 7
				addCorner(target, 0, o, 0, mag-1);
				enqueue(fx, fy+o, fz, mag-1, dir);
				addCorner(target, 0, o, o, mag-1);
				enqueue(fx, fy+o, fz+o, mag-1, dir);
				addCorner(target, o, o, 0, mag-1);
				enqueue(fx+o, fy+o, fz, mag-1, dir);
				addCorner(target, o, o, o, mag-1);
				enqueue(fx+o, fy+o, fz+o, mag-1, dir);
			}
			if(dir == 3){//000, 001, 100, 101
				//0, 1, 4, 5
				addCorner(target, 0, 0, 0, mag-1);
				enqueue(fx, fy, fz, mag-1, dir);
				addCorner(target, 0, 0, o, mag-1);
				enqueue(fx, fy, fz+o, mag-1, dir);
				addCorner(target, o, 0, 0, mag-1);
				enqueue(fx+o, fy, fz, mag-1, dir);
				addCorner(target, o, 0, o, mag-1);
				enqueue(fx+o, fy, fz+o, mag-1, dir);
			}
			if(dir == 4){//001, 011, 101, 111
				//1, 3, 5, 7
				addCorner(target, 0, 0, o, mag-1);
				enqueue(fx, fy, fz+o, mag-1, dir);
				addCorner(target, 0, o, o, mag-1);
				enqueue(fx, fy+o, fz+o, mag-1, dir);
				addCorner(target, o, 0, o, mag-1);
				enqueue(fx+o, fy, fz+o, mag-1, dir);
				addCorner(target, o, o, o, mag-1);
				enqueue(fx+o, fy+o, fz+o, mag-1, dir);
			}
			if(dir == 5){//000, 010, 100, 110
				//0, 2, 4, 6
				addCorner(target, 0, 0, 0, mag-1);
				enqueue(fx, fy, fz, mag-1, dir);
				addCorner(target, 0, o, 0, mag-1);
				enqueue(fx, fy+o, fz, mag-1, dir);
				addCorner(target, o, 0, 0, mag-1);
				enqueue(fx+o, fy, fz, mag-1, dir);
				addCorner(target, o, o, 0, mag-1);
				enqueue(fx+o, fy+o, fz, mag-1, dir);
			}

			//FIXME is from here:
/*			int full;
			oct* retDuplicate = getSubOctree(ret, fx, fy, fz, mag, &full);
			if(retDuplicate == NULL){
				if(full){
					//FIXME fill in the frontier and warn that this shouldn't happen
				}
			}else{
				unionOctree(target, retDuplicate);
			}*/
			//to here necessary or not?
		}
//		printOctree(f);
//		getchar();
	}
	freeOctree(f);
	printf("contiguous found a volume of %ld\n", totalSize);
	return ret;
}
