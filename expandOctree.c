#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "octreeOps.h"

static int xoff[26] = {-1,-1,-1,-1,-1,-1,-1,-1,-1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1};
static int yoff[26] = {-1,-1,-1, 0, 0, 0, 1, 1, 1,-1,-1,-1, 0, 0, 1, 1, 1,-1,-1,-1, 0, 0, 0, 1, 1, 1};
static int zoff[26] = {-1, 0, 1,-1, 0, 1,-1, 0, 1,-1, 0, 1,-1, 1,-1, 0, 1,-1, 0, 1,-1, 0, 1,-1, 0, 1};
//all tests occur against the original (ref), with t being altered.
void expandOctreeRec(oct* t, oct* ref){
	int sidelen = 1<<(t->mag);
	int csidelen = 1<<(t->mag-1);
	//if it is full, check the surrounding cubes. If any are not full, make this partial with full children.
	if(t->full && t->mag >= 2){
		int* c = t->corner;
		char partialize = 0;
		for(int oidx = 0; oidx < 26; oidx++){
			int tx = c[0]+xoff[oidx]*sidelen;
			int ty = c[1]+yoff[oidx]*sidelen;
			int tz = c[2]+zoff[oidx]*sidelen;
			//if a neighbor isn't full
			if(2 != cornerExists(ref, tx, ty, tz, t->mag, NULL)){
				partialize = 1;
				break;
			}
		}
		//make this partial, and add 8 full children
		if(partialize){
			t->full = 0;
			for(int cidx = 0; cidx < 8; cidx++){
				oct* newc = malloc(sizeof(oct));
				newc->mag = t->mag-1;
				newc->full = 1;
				getCubeCorner(cidx, t, newc->corner);
				memset(newc->child, 0, 8*sizeof(oct*));
				t->child[cidx] = newc;
			}
		}
	}
	//cannot be elif because above if statement may remove t's full status
	if(!t->full){
		oct** c = t->child;
		for(int cidx = 0; cidx < 8; cidx++){
			//if this child is empty, check the surrounding cubes. If any are not empty, make this partial.
			if(c[cidx] == NULL && t->mag-1 >= 2){
				int ccorner[3];
				getCubeCorner(cidx, t, ccorner);
				char partialize = 0;
				for(int oidx = 0; oidx < 26; oidx++){
					int cl[3] = {ccorner[0]+xoff[oidx]*csidelen, ccorner[1]+yoff[oidx]*csidelen, ccorner[2]+zoff[oidx]*csidelen};
					//if a neighbor of this child isn't empty (0 or -1)
					if(0 < cornerExists(ref, cl[0], cl[1], cl[2], t->mag-1, NULL)){
						partialize = 1;
						break;
					}
				}
				//put an 'empty' partial cube here (partial but with no children). This only makes sense in the context of octree expansion.
				if(partialize){
					oct* newc = malloc(sizeof(oct));
					newc->mag = t->mag-1;
					newc->full = 0;
					getCubeCorner(cidx, t, newc->corner);
					memset(newc->child, 0, 8*sizeof(oct*));
					c[cidx] = newc;
				}
			}
			if(c[cidx]){
				expandOctreeRec(c[cidx], ref);
			}
		}
	}
}
oct* expandOctree(oct* t){
	fprintf(stderr, "called expandOctree\n");
	if(t == NULL) return NULL;
	oct* ret = duplicateOctree(t);
	expandOctreeRec(ret, t);
	return ret;
}
