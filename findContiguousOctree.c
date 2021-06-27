#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "octreeOps.h"

typedef struct fRef{
	struct fRef* next;
	pt loc;
	unsigned short mag;
	char dir;//x-, x+, y-, y+, z-, z+. this way, when we have to downgrade our magnitude, we know which four to add
}fRef;
long int queueLen = 0;
fRef *fEnqueue = NULL, *fDequeue = NULL;
void enqueue(pt loc, unsigned short mag, char dir){
	queueLen++;
	if(fEnqueue == NULL){
		fEnqueue = malloc(sizeof(fRef));
		fDequeue = fEnqueue;
	}else{
		fEnqueue->next = malloc(sizeof(fRef));//FIXME this shouldn't be duplicated. sloppy sloppy
		fEnqueue = fEnqueue->next;
	}
	fEnqueue->loc = loc;
	fEnqueue->mag = mag;
	fEnqueue->dir = dir;
	fEnqueue->next = NULL;
}
void prependenqueue(pt loc, unsigned short mag, char dir){
	queueLen++;
	fRef* prevDequeue = fDequeue;
	fDequeue = malloc(sizeof(fRef));
	if(fEnqueue == NULL){
		fEnqueue = fDequeue;
	}
	fDequeue->loc = loc;
	fDequeue->mag = mag;
	fDequeue->dir = dir;
	fDequeue->next = prevDequeue;
}
void dequeue(pt* loc, unsigned short *mag, char *dir){
	queueLen--;
	*loc = fDequeue->loc;
	*mag = fDequeue->mag;
	*dir = fDequeue->dir;
	if(fDequeue == fEnqueue){
		fEnqueue = NULL;
	}
	fRef* old = fDequeue;
	fDequeue = fDequeue->next;
	free(old);
}

oct* findContiguousOctree(oct* t, pt corner){
	fprintf(stderr, "findContiguousOctree called\n");
	//check if that corner exists
	int initialCorner = cornerExists(t, corner, 0, NULL, NULL);
	if(initialCorner == -1){
		fprintf(stderr, "ERROR: findContiguousOctree: initial corner (%ld %ld %ld) invalid\n", corner.l[0], corner.l[1], corner.l[2]);
		return NULL;
	}else if(initialCorner != 2){
		fprintf(stderr, "ERROR: findContiguousOctree: initial corner doesn't exist\n");
		return NULL;
	}
	//We store the frontier as an octree, since it is easily searchable, and easily editable. In addition, we keep a queue of locations for proper frontier action. Keep in mind that the frontier octree also includes all locations that have ever been part of the frontier. By the end, it will be identical to the returned octree, plus an outer shell.
	oct* f = createEmptyOct(t->mag);
	subtree froot = rootSubtree(f);
	treechain* tchain = createTreechain(t); 
	oct* ret = createEmptyOct(t->mag);
	addCornerRec(&froot, corner, 0);
	enqueue(corner, 0, 0);
	//frontier expansion loop
	//while there are still unexplored frontiers
	while(fDequeue != NULL){
		pt loc;
		unsigned short mag;
		char dir;
		dequeue(&loc, &mag, &dir);
		int foundMag;
		pt foundCorner;
		int cornerExistsRet = cornerExistsTC(tchain, loc, mag, &foundMag, &foundCorner);
		//exists fully
		if(cornerExistsRet == 2){
			subtree fmarginParent = marginParentSubtree(&froot, foundCorner, foundMag);
			long int sideLen = sidelen(foundMag);
			if(foundMag > mag){
				addCornerRec(&fmarginParent, foundCorner, foundMag);
			}
			addCorner(ret, foundCorner, foundMag);
			for(int axis = 0; axis < DIM; axis++){
				for(int direff = -1; direff <= 1; direff += 2){
					pt alteredpt = foundCorner;
					alteredpt.l[axis] += direff*sideLen;
					int fCornerExists = cornerExistsRec(&fmarginParent, alteredpt, foundMag, NULL, NULL);
					if(fCornerExists == 0){
						addCornerRec(&fmarginParent, alteredpt, foundMag);
						enqueue(alteredpt, foundMag, axis*2+(direff+1)/2);
					}
				}
			}
		}else if(cornerExistsRet == 1){
			subtree fmarginParent = marginParentSubtree(&froot, loc, mag);
			deleteCorner(&fmarginParent, loc, mag);
			long int o = sidelen(mag-1);//offset
			// 0,1->0; 2,3->1; 4,5->2
			int primaryaxis = dir/2;
			int otheraxes[DIM-1];
			for(int idx = 1; idx < DIM; idx++){
				otheraxes[idx-1] = (primaryaxis+idx)%DIM;
			}
			//evens->1, odds->0
			int primaryside = 1^(dir%2);
			//we can compress the margin parent a bit closer
			fmarginParent = addCornerRec(&fmarginParent, loc, mag);
			setStatus(&fmarginParent, 'P', 'E');
			for(int idx = 0; idx < (1<<(DIM-1)); idx++){
				pt alteredpt = loc;
				alteredpt.l[primaryaxis] += primaryside*o;
				for(int aidx = 0; aidx < DIM-1; aidx++){
					alteredpt.l[otheraxes[aidx]] += o*((idx/(aidx+1))%2);
				}
				addCornerRec(&fmarginParent, alteredpt, mag-1);
				//prepend gives better locality for caching
				prependenqueue(alteredpt, mag-1, dir);
			}
		}
	}
	freeTreechain(tchain);
	freeOctree(f);
	return ret;
}
