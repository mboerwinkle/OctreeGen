#ifndef STRUCTURES_H
#define STRUCTURES_H
#include <inttypes.h>

extern double resolution;
extern int magnitude;
extern char expandflag;
extern char stlflag;
typedef struct facet{
	float vec[3];
	float p1[3];
	float p2[3];
	float p3[3];
	uint16_t attr;
}__attribute__((packed)) facet;
typedef struct model{
	facet* facets;
	uint32_t facetCount;
	float min[3];
	float max[3];
}model;



#define DIM 3
#define TWOPOWDIM (1 << DIM)

typedef struct pt{
	long int l[DIM];
}pt;
typedef struct oct{
	unsigned long int tlen;
	unsigned long int tusage;
	char* t;
	//even char is way overkill
	unsigned short mag;
}oct;
typedef struct subtree{
	oct* origin;
	//index in origin->t
	unsigned long int offset;
	pt corner;
	unsigned short mag;
}subtree;


extern oct* createEmptyOct(unsigned short magnitude);
extern oct* duplicateOctree(oct* t);
extern oct* invertOctree(oct* t);
extern void freeOctree(oct* t);
extern void validateOctree(oct* t);
extern long int sidelen(unsigned short mag);
extern void enableIndex(unsigned long int offset, oct* target);
extern char getAtIndex(unsigned long int offset, oct* target);
extern void setAtIndex(unsigned long int offset, oct* target, char value);
extern void insertAtIndex(unsigned long int offset, oct* target, char val);
extern void deleteAtIndex(unsigned long int offset, oct* target);
extern pt getParentCorner(char cidx, unsigned short cmag, pt ccorner);
extern pt getCubeCorner(char cidx, unsigned short pmag, pt pcorner);

extern char getStatus(subtree* target);
extern void setStatus(subtree* target, char status, char cstatus);
extern void printSubtreeStatus(subtree* target);
extern subtree rootSubtree(oct* target);
extern subtree cornerSubtree(oct* target, pt corner, unsigned short mag);
extern subtree childSubtree(subtree* target, char cidx);
extern subtree siblingSubtree(subtree* target, char tcidx);
extern subtree marginParentSubtree(subtree* target, pt corner, unsigned short mag);
extern void deleteCorner(subtree* t, pt loc, unsigned short mag);
extern void clearSubtree(subtree* target);
#endif
