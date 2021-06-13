#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "structures.h"

extern void processFile(FILE* inputFile, FILE* outputFile, double modelSize);
int main(int argc, char** argv){
	FILE *inputFile = NULL, *outputFile = NULL;
	float stldimsize = -1.0;
	#ifndef NDEBUG
		printf("oct size: %ld bytes\n", sizeof(oct));
	#endif
	static struct option long_options[] = {
		{"help", no_argument, NULL, 'h'},
		{"input", required_argument, NULL, 'i'},
		{"output", required_argument, NULL, 'o'},
		{"resolution", required_argument, NULL, 'r'},
		{"size", required_argument, NULL, 's'},
		{0,0,0,0}
	};
	while(1){
		int option_index = 0;
		int c = getopt_long(argc, argv, "hi:o:r:s:", long_options, &option_index);
		if(c == -1) break;
		switch(c){
			case 'i':
				inputFile = fopen(optarg, "r");
				if(inputFile == NULL){
					printf("failed to open %s\n", optarg);
				}
				break;
			case 'o':
				outputFile = fopen(optarg, "w");
				if(outputFile == NULL){
					printf("failed to open %s\n", optarg);
				}
				break;
			case 'r':
				resolution = atof(optarg);
				printf("Resolution set to %lf\n", resolution);
				break;
			case 's':
				stldimsize = atof(optarg);
				printf("STL dimension size set to %lf\n", stldimsize);
				break;
			case 'h':
			case '?':
			default:
				printf("USAGE:\n-h,--help\n-i,--input <input STL path> (REQUIRED)\n-o,--output <output octree path> (REQUIRED)\n-r,--resolution <real-valued smallest voxel size>\n-s,--size <real-valued longest STL dimension target size>\n");
				return -1;
		}
	}
	if(!inputFile || !outputFile){
		printf("Both input and output must be specified. Try --help.\n");
		return 1;
	}
	processFile(inputFile, outputFile, stldimsize);
	return 0;
}
