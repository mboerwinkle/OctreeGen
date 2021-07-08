#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "structures.h"

char helptext[] = "\
USAGE:\n\
-h,--help\n\
-i,--input <input STL path> (REQUIRED) ('-i -' for stdin.)\n\
-o,--output <output octree path> (REQUIRED) ('-o -' for stdout. Autodetects format as binary .stl based on extension. Assumes octree if not stl.)\n\
-r,--resolution <real-valued smallest voxel size>\n\
-s,--size <real-valued longest STL dimension target size>\n\
-e,--expand (Unsuitable for most applications. Makes full cubes partial if they are not fully surrounded by full cubes. Makes empty cubes partial if they have a full or partial neighbor.)\n\
";
char stlflag = 0;
extern void processFile(FILE* inputFile, FILE* outputFile, double modelSize);
char chrlwr(char i){
	return (i >= 'A' && i <= 'Z')?(i+32):i;
}
int main(int argc, char** argv){
	FILE *inputFile = NULL, *outputFile = NULL;
	float stldimsize = -1.0;
	static struct option long_options[] = {
		{"help", no_argument, NULL, 'h'},
		{"input", required_argument, NULL, 'i'},
		{"output", required_argument, NULL, 'o'},
		{"resolution", required_argument, NULL, 'r'},
		{"size", required_argument, NULL, 's'},
		{"expand", no_argument, NULL, 'e'},
		{0,0,0,0}
	};
	static char short_options[] = "hi:o:r:s:e";
	while(1){
		int option_index = 0;
		int c = getopt_long(argc, argv, short_options, long_options, &option_index);
		if(c == -1) break;
		int optlen = (optarg)?strlen(optarg):0;
		switch(c){
			case 'i':
				if(strcmp(optarg, "-") == 0){
					inputFile = stdin;
				}else{
					inputFile = fopen(optarg, "r");
				}
				if(inputFile == NULL){
					fprintf(stderr, "failed to open %s\n", optarg);
					return 10;
				}
				break;
			case 'o':
				if(optlen >= 3 && chrlwr(optarg[optlen-3]) == 's' && chrlwr(optarg[optlen-2]) == 't' && chrlwr(optarg[optlen-1]) == 'l'){
					stlflag = 1;
					fprintf(stderr, "STL Output Mode\n");
				}else fprintf(stderr, "Octree Output Mode\n");
				if(strcmp(optarg, "-") == 0){
					outputFile = stdout;
				}else{
					outputFile = fopen(optarg, "w");
				}
				if(outputFile == NULL){
					fprintf(stderr, "failed to open %s\n", optarg);
					return 11;
				}
				break;
			case 'r':
				resolution = atof(optarg);
				fprintf(stderr, "Resolution set to %lf\n", resolution);
				break;
			case 's':
				stldimsize = atof(optarg);
				fprintf(stderr, "STL dimension size set to %lf\n", stldimsize);
				break;
			case 'e':
				expandflag = 1;
				fprintf(stderr, "Expand ON\n");
				break;
			case 'h':
			case '?':
			default:
				fprintf(stderr, helptext);
				return -1;
		}
	}
	if(!inputFile || !outputFile){
		fprintf(stderr, "Both input and output must be specified. Try --help.\n");
		return 1;
	}
	processFile(inputFile, outputFile, stldimsize);
	return 0;
}
