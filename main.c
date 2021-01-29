#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"

extern void processFile(FILE* inputFile, FILE* outputFile, double modelSize);
int main(int argc, char** argv){
	FILE *inputFile, *outputFile;
	printf("oct size: %ld bytes\n", sizeof(oct));
	if(argc == 5){
		inputFile = fopen(argv[1], "r");
		if(inputFile == NULL){
			printf("failed to open %s\n", argv[1]);
			return 1;
		}
		if(argc > 2){//if a second argument is given, it is the output file
			outputFile = fopen(argv[2], "w");
			if(argc > 3){
				resolution = atof(argv[3]);
				printf("Resolution set to %lf\n", resolution);
			}
		}else{
			outputFile = stdout;
		}
		processFile(inputFile, outputFile, atof(argv[4]));
	}else{//usage
		puts("Usage: ./run inputfile outputfile resolution modelsize");
		return 1;
	}
	return 0;
}
