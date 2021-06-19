#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "structures.h"
#include "octreeOps.h"

oct* createEmptyOct(unsigned short magnitude){
	oct* ret = malloc(sizeof(oct));
	ret->mag = magnitude;
	ret->t = NULL;
	ret->tlen = 0;
	ret->tusage = 0;
	enableIndex(100, ret);
	subtree originSubtree = rootSubtree(ret);
	setStatus(&originSubtree, 'P', 'E');
	return ret;
}
oct* duplicateOctree(oct* t){
	oct* ret = malloc(sizeof(oct));
	ret->mag = t->mag;
	ret->tlen = t->tlen;
	ret->tusage = t->tusage;
	ret->t = malloc(ret->tlen);
	memcpy(ret->t, t->t, ret->tlen);
	return ret;
}
oct* invertOctree(oct* t){
	fprintf(stderr, "invertOctree called\n");
	oct* ret = malloc(sizeof(oct));
	ret->mag = t->mag;
	ret->tlen = t->tlen;
	ret->tusage = t->tusage;
	ret->t = malloc(ret->tlen);
	for(int idx = 0; idx < ret->tusage; idx++){
		char val = t->t[idx];
		ret->t[idx] = (val == 'P')?'P':(val == 'E'?'F':'E');
	}
	return ret;
}
void freeOctree(oct* t){
	free(t->t);
	free(t);
}
void validateSubtree(subtree* t){
	#ifndef NDEBUG
	char status = getStatus(t);
	assert(status == 'E' || status == 'P' || status == 'F');
	if(status == 'E' || status == 'F') return;
	assert(status == 'P');
	assert(t->mag > 0);
	int nonFull = 0;
	int nonEmpty = 0;
	for(int cidx = 0; cidx < TWOPOWDIM; cidx++){
		subtree child = childSubtree(t, cidx);
		char cstatus = getStatus(&child);
		if(cstatus != 'F') nonFull = 1;
		if(cstatus != 'E') nonEmpty = 1;
		validateSubtree(&child);
	}
	//no partial cube should be composed of exclusively full or empty cubes
	assert(nonFull && nonEmpty);
	#endif
}
void validateOctree(oct* t){
	subtree root = rootSubtree(t);
	validateSubtree(&root);
}

void printSubtreeStatus(subtree* target){
	oct* r = target->origin;
	char s[r->tusage+1];
	strncpy(s, r->t, r->tusage);
	s[r->tusage] = 0;
	fprintf(stderr, "%s\n", s);
	for(int i = 0; i < target->offset; i++){
		s[i] = ' ';
	}
	s[target->offset] = '^';
	s[target->offset+1] = '\0';
	fprintf(stderr, "%s\n", s);
}

long int sidelen(unsigned short mag){
	assert(mag < 63);
	return ((unsigned long int) 1) << mag;
}

void enableIndex(unsigned long int offset, oct* target){
	if(offset >= target->tlen){
		target->tlen = offset * 1.5 + 1;
		target->t = realloc(target->t, target->tlen);
	}
}
char getAtIndex(unsigned long int offset, oct* target){
	assert(offset < target->tusage);
	#ifndef NDEBUG
		char val = target->t[offset];
	#endif
	assert(val == 'F' || val == 'E' || val == 'P');
	return target->t[offset];
}
void setAtIndex(unsigned long int offset, oct* target, char val){
	assert(offset <= target->tusage);
	if(offset == target->tusage){
		target->tusage = offset+1;
	}
	assert(val == 'F' || val == 'E' || val == 'P');
	enableIndex(offset, target);
	target->t[offset] = val;
}

void insertAtIndex(unsigned long int offset, oct* target, char val){
	assert(val == 'F' || val == 'E' || val == 'P');
	if(target->tlen == target->tusage){
		enableIndex(target->tlen, target);
	}
	memmove(target->t+offset+1, target->t+offset, target->tusage-offset);
	target->tusage ++;
	target->t[offset] = val;
}
void deleteAtIndex(unsigned long int offset, oct* target){
	memmove(target->t+offset, target->t+offset+1, target->tusage-offset-1);
	target->tusage --;
	target->t[target->tusage] = 0;
}
char getStatus(subtree* target){
	return getAtIndex(target->offset, target->origin);
}

void clearSubtree(subtree* target){
	assert(getStatus(target) == 'P');
	subtree child;
	for(int cidx = 0; cidx < TWOPOWDIM; cidx++){
		child = childSubtree(target, 0);
		char cval = getStatus(&child);
		if(cval == 'P'){
			clearSubtree(&child);
		}
		deleteAtIndex(child.offset, target->origin);
	}
}

void setStatus(subtree* target, char status, char cstatus){
	assert(!(status == 'P' && target->mag == 0));
	if(target->offset < target->origin->tusage){
		char oldstatus = getStatus(target);
		assert(oldstatus != status);
		if(oldstatus == 'P'){
			clearSubtree(target);
		}
	}
	setAtIndex(target->offset, target->origin, status);
	if(status == 'P'){
		assert(cstatus == 'E' || cstatus == 'F');
		for(int idx = 0; idx < TWOPOWDIM; idx++){
			insertAtIndex(target->offset+1, target->origin, cstatus);
		}
	}
}

subtree rootSubtree(oct* target){
	long int co = -sidelen(target->mag-1);
	return (subtree) {
		.origin = target,
		.offset = 0,
		.corner = {.l = {co,co,co}},
		.mag = target->mag
	};
}
subtree cornerSubtree(oct* target, pt corner, unsigned short mag){
	subtree out = rootSubtree(target);
	while(out.mag != mag){
		out = childSubtree(&out, identifyCorner(&out, corner));
	}
	return out;
}
subtree childSubtree(subtree* target, char cidx){
	assert(getStatus(target) == 'P' && cidx >= 0 && cidx < TWOPOWDIM);
	char* data = &(target->origin->t[target->offset+1]);
	for(unsigned long int remainingsteps = cidx; remainingsteps; remainingsteps--){
		if(*data == 'P') remainingsteps += TWOPOWDIM;
		data++;
	}
	return (subtree) {
		.origin = target->origin,
		.offset = data - target->origin->t,
		.corner = getCubeCorner(cidx, target->mag, target->corner),
		.mag = target->mag-1
	};
}
subtree siblingSubtree(subtree* target, char tcidx){
	assert(tcidx >= 0 && tcidx < 7);
	char* data = &(target->origin->t[target->offset]);
	for(unsigned long int remainingsteps = 1; remainingsteps; remainingsteps--){
		if(*data == 'P') remainingsteps += TWOPOWDIM;
		data++;
	}
	return (subtree) {
		.origin = target->origin,
		.offset = data - target->origin->t,
		.corner = getCubeCorner(tcidx+1, target->mag+1, getParentCorner(tcidx, target->mag, target->corner)),
		.mag = target->mag
	};
}
void deleteCorner(oct* t, pt loc, unsigned short mag){
	subtree curr = rootSubtree(t);
	while(curr.mag != mag){
		char status = getStatus(&curr);
		//it already doesn't exist
		if(status == 'E') return;
		//we need to subdivide it
		if(status == 'F'){
			setStatus(&curr, 'P', 'F');
		}
		curr = childSubtree(&curr, identifyCorner(&curr, loc));
	}
	setStatus(&curr, 'E', 0);
}
pt getParentCorner(char cidx, unsigned short cmag, pt ccorner){
	long int sideLen = sidelen(cmag);
	return (pt){.l = {
		ccorner.l[0]-((cidx < 4)?0:sideLen),
		ccorner.l[1]-((cidx < 2 || (cidx >= 4 && cidx < 6))?0:sideLen),
		ccorner.l[2]-((cidx%2 == 0)?0:sideLen)
	}};
}
pt getCubeCorner(char cidx, unsigned short pmag, pt pcorner){
	long int sidelen2 = sidelen(pmag-1);
	return (pt){.l = {
		pcorner.l[0]+((cidx < 4)?0:sidelen2),
		pcorner.l[1]+((cidx < 2 || (cidx >= 4 && cidx < 6))?0:sidelen2),
		pcorner.l[2]+((cidx%2 == 0)?0:sidelen2)
	}};
}

