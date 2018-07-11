#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
#include "octreeOps.h"
int cornerExistsVerbose = 0;//FIXME delete this shit
int cornerExists(oct* t, int x, int y, int z){
	if(t == NULL){
		puts("does not exist, null.");
		return 0;
	}
	int sideLen = 1<<(t->mag);
	if(x < 0 || y < 0 || z < 0 || x >= sideLen || y >= sideLen || z >= sideLen){
		puts("invalid position");
		return -1;//invalid
	}
	if(t->full){
		puts("exists, full.");
		return 1;
	}
	int cIdx = identifyCorner(t, x, y, z);
	//this little chunk of code is the translation to child local coordinates
	sideLen /= 2;
	if(x >= sideLen) x-= sideLen;
	if(y >= sideLen) y-= sideLen;
	if(z >= sideLen) z-= sideLen;
	return cornerExists(t->child[cIdx], x, y, z);//FIXME use turnaries?
}
int identifyCorner(oct* t, int x, int y, int z){
	int edgeLen = 1<<(t->mag-1);
	x/=edgeLen;
	y/=edgeLen;
	z/=edgeLen;
	if(x){
		if(y){
			if(z){
				return 7;
			}else{
				return 6;
			}
		}else{
			if(z){
				return 5;
			}else{
				return 4;
			}
		}
	}else{
		if(y){
			if(z){
				return 3;
			}else{
				return 2;
			}
		}else{
			if(z){
				return 1;
			}else{
				return 0;
			}
		}
	}
}
