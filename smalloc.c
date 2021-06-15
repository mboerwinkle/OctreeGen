#include <stdio.h>
#include <stdlib.h>

void* smalloc(size_t size){
	void* ret = malloc(size);
	if(ret == NULL){
		fprintf(stderr, "malloc returned NULL\n");
	}
	return ret;
}
