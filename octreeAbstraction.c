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
	setStatus(&originSubtree, 'P');
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

void setStatus(subtree* target, char status){
	assert(!(status == 'P' && target->mag == 0));
	if(target->offset < target->origin->tusage){
		char oldstatus = getStatus(target);
		if(oldstatus == status) return;
		if(oldstatus == 'P'){
			clearSubtree(target);
		}
	}
	setAtIndex(target->offset, target->origin, status);
	if(status == 'P'){
		for(int idx = 0; idx < TWOPOWDIM; idx++){
			insertAtIndex(target->offset+1, target->origin, 'E');
		}
	}
}

unsigned long int getSiblingOffset(unsigned long int offset, char* data){
	unsigned long int remainingsteps = 1;
	unsigned long int coffset = offset;
	while(remainingsteps){
		remainingsteps--;
		if(data[coffset] == 'P') remainingsteps += TWOPOWDIM;
		coffset++;
	}
	return coffset;
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
	unsigned long int childoffset = target->offset+1;
	for(int idx = 0; idx < cidx; idx++){
		childoffset = getSiblingOffset(childoffset, target->origin->t);
	}
	return (subtree) {
		.origin = target->origin,
		.offset = childoffset,
		.corner = getCubeCorner(cidx, target->mag, target->corner),
		.mag = target->mag-1
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
			setStatus(&curr, 'P');
			//and set all children to full
			for(int cidx = 0; cidx < TWOPOWDIM; cidx++){
				subtree child = childSubtree(&curr, cidx);
				setStatus(&child, 'F');
			}
		}
		curr = childSubtree(&curr, identifyCorner(&curr, loc));
	}
	assert(curr.mag == mag);
	setStatus(&curr, 'E');
}
pt getCubeCorner(char cidx, unsigned short pmag, pt pcorner){
	long int sidelen2 = sidelen(pmag-1);
	pt pcenter = {.l = {pcorner.l[0]+sidelen2, pcorner.l[1]+sidelen2, pcorner.l[2]+sidelen2}};
	return (pt){.l = {(cidx < 4)?pcorner.l[0]:pcenter.l[0] , (cidx < 2 || (cidx >= 4 && cidx < 6))?pcorner.l[1]:pcenter.l[1] , (cidx%2 == 0)?pcorner.l[2]:pcenter.l[2]}};
}

