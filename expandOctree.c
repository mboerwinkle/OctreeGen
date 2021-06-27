#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "structures.h"
#include "octreeOps.h"

static int xoff[26] = {-1,-1,-1,-1,-1,-1,-1,-1,-1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1};
static int yoff[26] = {-1,-1,-1, 0, 0, 0, 1, 1, 1,-1,-1,-1, 0, 0, 1, 1, 1,-1,-1,-1, 0, 0, 0, 1, 1, 1};
static int zoff[26] = {-1, 0, 1,-1, 0, 1,-1, 0, 1,-1, 0, 1,-1, 1,-1, 0, 1,-1, 0, 1,-1, 0, 1,-1, 0, 1};
static int* off[3] = {xoff, yoff, zoff};
//all tests occur against the original (ref), with t being altered.
void expandOctreeRec(subtree* t, subtree* ref){
	if(t->mag < 1) return;
	long int sideLen = sidelen(t->mag);
	long int csideLen = sidelen(t->mag-1);
	char status = getStatus(t);
	//if it is full, check the surrounding cubes. If any are not full, make this partial with full children.
	if(status == 'F'){
		char partialize = 0;
		for(int oidx = 0; oidx < 26; oidx++){
			pt test = t->corner;
			for(int dim = 0; dim < DIM; dim++){
				test.l[dim] += off[dim][oidx]*sideLen;
			}
			//if a neighbor isn't full
			if(2 != cornerExistsRec(ref, test, t->mag, NULL, NULL)){
				partialize = 1;
				break;
			}
		}
		assert(2 == cornerExistsRec(ref, t->corner, t->mag, NULL, NULL));
		//make this partial, and add 8 full children
		if(partialize){
			setStatus(t, 'P', 'F');
		}
	}
	//above if statement may remove t's full status, so we need to recheck
	status = getStatus(t);
	if(status == 'P'){
		subtree child = childSubtree(t, 0);
		for(int cidx = 0; cidx < 8; cidx++){
			if(cidx > 0) child = siblingSubtree(&child, cidx-1);
			char cstatus = getStatus(&child);
			//if this child is empty, check the surrounding cubes. If any are not empty, make this partial.
			if(cstatus == 'E' && t->mag-1 >= 1){
				char partialize = 0;
				for(int oidx = 0; oidx < 26; oidx++){
					pt cl = child.corner;
					for(int dim = 0; dim < 3; dim++){
						cl.l[dim] += off[dim][oidx]*csideLen;
					}
					//if a neighbor of this child isn't empty (0 or -1)
					if(0 < cornerExistsRec(ref, cl, t->mag-1, NULL, NULL)){
						partialize = 1;
						break;
					}
				}
				//put an 'empty' partial cube here (partial but with no children). This only makes sense in the context of octree expansion.
				if(partialize){
					setStatus(&child, 'P', 'E');
				}
			}
			cstatus = getStatus(&child);
			if(cstatus == 'P' || cstatus == 'F'){
				subtree refMarginParent = marginParentSubtree(ref, child.corner, child.mag);
				expandOctreeRec(&child, &refMarginParent);
			}
		}
	}
}
oct* expandOctree(oct* t){
	fprintf(stderr, "called expandOctree\n");
	oct* ret = duplicateOctree(t);
	subtree retroot = rootSubtree(ret);
	subtree troot = rootSubtree(t);
	expandOctreeRec(&retroot, &troot);
	return ret;
}
