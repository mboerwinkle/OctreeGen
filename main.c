#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"

extern void processFile(FILE* inputFile, FILE* outputFile);
int main(int argc, char** argv){
	FILE *inputFile, *outputFile;
	printf("oct size: %ld\n", sizeof(oct));
	if(argc == 1){//process everything
		FILE* contentsFp = fopen("contents.txt", "r");
		if(contentsFp == NULL){
			puts("contents.txt failed to open");
			return 1;
		}
		char filename[110];
		while(EOF != fscanf(contentsFp, "%s", filename)){//FIXME this block is nasty AF
			char extFilename[110];//including extension
			strncpy(extFilename, filename, 90);
			strcat(extFilename, ".stl");
			inputFile = fopen(extFilename, "r");
			if(inputFile == NULL){
				printf("failed to open %s\n", extFilename);
				continue;
			}
			strncpy(extFilename, filename, 90);
			strcat(extFilename, "_Voxel.txt");
			outputFile = fopen(extFilename, "w");
			processFile(inputFile, outputFile);
		}
	}else if(argc >= 2 && argc < 5){//process only indicated file
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
		processFile(inputFile, outputFile);
	}else{//usage
		puts("Usage: ./run [input file [output file [resolution]]]");
		return 1;
	}
	return 0;
}
